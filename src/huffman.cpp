#include <huffman.h>

namespace avlib
{

CHuffmanCode::CHuffmanCode():Bits(NULL),Length(0),Size(0),bits(0){}

CHuffmanCode::~CHuffmanCode()
{
	if(Bits != NULL)
		delete Bits;
}

void CHuffmanCode::Put(bool bit)
{
	if(Length%32 == 0)
	{
		uint32_t * temp = new uint32_t[Size+1];
		for(int8_t i = 0 ; i < Size;i++)
			temp[i] = Bits[i];
		temp[Size++] = 0;
		delete Bits;
		Bits = temp;
	}
	if(bit)
		Bits[Size-1] |= 1<<(Length%32);
	Length++;
}

void CHuffmanCode::Write(CBitstream * bitstream)
{
	for(int8_t i=0;i<Size-1;i++)
		bitstream->putBits(32,Bits[i]);
	bitstream->putBits(Length%32,Bits[Size-1]);
}

template <class T> 
CHuffmanTree<T>::CHuffmanTree() 
{
	this->symbol_size = 8*sizeof(T);
	this->Init();
}

template <class T> 
CHuffmanTree<T>::CHuffmanTree(const CHuffmanTree<T> & source)
{
	this->symbol_size = source.symbol_size;
	this->First = NULL;
	this->Last = NULL;
	this->Copy(source);
}

template <class T> 
CHuffmanTree<T>::~CHuffmanTree(void) 
{
	this->DeleteAll();
}

template <class T>
void CHuffmanTree<T>::setSymbolSize(uint8_t ss)
{
	this->symbol_size = ss;
}

template <class T>
uint8_t CHuffmanTree<T>::getSymbolSize()
{
	return this->symbol_size;
}

template <class T> 
void CHuffmanTree<T>::Copy(const CHuffmanTree<T> & source)
{
	if(this->First || this->Last)
		this->DeleteAll();
	this->Last = new CHuffmanItem<T>(*source.Last);
	this->leafCounter = source.leafCounter;
	CHuffmanItem<T> * p = this->Last;
	uint32_t n = 1;
	while(n < source.First->SN)
	{
		if(p->Prev == NULL)
		{
			p->Prev = this->Last->FindN(n+1);
			p->Prev->Next = p;
			
		}
		p=p->Prev;
		n++;		
	}
	this->First = p;
}

template <class T> 
void CHuffmanTree<T>::Init()				//one node and two leafs: EOB and New at the beggining
{
	CHuffmanItem<T> * iEOB = new CHuffmanItem<T>(0,1,NULL,NULL,NULL,NULL,EOB,NULL,3);
	CHuffmanItem<T> * iNew = new CHuffmanItem<T>(0,1,NULL,NULL,NULL,NULL,New,NULL,2);
	CHuffmanItem<T> * iNode = new CHuffmanItem<T>(0,2,NULL,NULL,iEOB,iNew,Node,iNew,1);
	iEOB->Next = iNew;
	iEOB->Parent = iNode;
	iNew->Next = iNode;
	iNew->Parent = iNode;
	iNew->Prev = iEOB;
	First = iEOB;
	Last = iNode;
	this->leafCounter = 0;
}

template <class T> 
void CHuffmanTree<T>::DeleteAll()
{
	CHuffmanItem<T> * p = NULL;
	CHuffmanItem<T> * next = NULL;
	for(p=First;p;p=next)
	{
		next = p->Next;
		delete p;
	}
	Last = NULL;
	First = NULL;
	this->leafCounter = 0;
}

template <class T> 
void CHuffmanTree<T>::Reset()
{
	this->DeleteAll();
	this->Init();
}

template <class T> 
void CHuffmanTree<T>::Insert(T symbol,HuffmanItemType itemtype)
{
	this->leafCounter++;
	CHuffmanItem<T> * u = new CHuffmanItem<T>(First->Value,First->Weight,First,First,First->Left,First->Right,First->ItemType,NULL,First->SN+1);
	CHuffmanItem<T> * v = new CHuffmanItem<T>(symbol,1,u,First,NULL,NULL,itemtype,NULL,First->SN+2);
	u->Prev = v;
	First->Value = 0;
	First->Left = v;
	First->Right = u;
	First->ItemType = Node;
	First->Weight = u->Weight;
	First->Prev = u;
	First = v;
	First->Next = u; 
	CHuffmanItem<T> * parent = First->Parent;
	if(parent)
		parent->IncWeightAndModify();
}

template <class T> 
bool CHuffmanTree<T>::EncodeCtl(HuffmanItemType itemtype,CBitstream * bitstream)
{
	CHuffmanItem<T> * v = this->FindItemType(itemtype);
	if(v==NULL)
	{
		fprintf(stderr,"Encode CTL v=null\n");
		return false;
	}
	if(bitstream != NULL)
	{
		CHuffmanCode * code = this->GetCode(v);
		code->Write(bitstream);
		delete code;
	}
	v->IncWeightAndModify();
	return true;
}

template <class T> 
bool CHuffmanTree<T>::EncodeBlock(T * ptr,uint32_t size,CBitstream * bstr,bool inc_new)
{
	while(size--)
		this->Encode(*(ptr++),bstr,inc_new);
	return true;
}

template <class T> 
HuffmanItemType CHuffmanTree<T>::DecodeBlock(T * ptr,uint32_t size,CBitstream * bstr)
{
	HuffmanItemType itype;
	T symbol;
	while(size--)
	{
		itype = this->Decode(bstr,&symbol);
		if(itype!=Leaf && itype != New)
			return itype;
		*(ptr++) = symbol;
	}
	return None;
}

template <class T> 
bool CHuffmanTree<T>::Encode(T symbol,CBitstream * bitstream,bool inc_new)	
{
	CHuffmanItem<T> * v = this->FindLeafValue(symbol);
	if(v == NULL)
	{
		CHuffmanItem<T> * newType = this->FindItemType(New);
		if(bitstream != NULL)
		{
			CHuffmanCode * code = this->GetCode(newType);
			code->Write(bitstream);
			delete code;			
			bitstream->putBits(this->symbol_size,(uint32_t)symbol);
		}
		if(inc_new)
			newType->IncWeightAndModify();
		this->Insert(symbol);
		return true;												
	}
	if(bitstream != NULL)
	{
		CHuffmanCode * code = this->GetCode(v);
		code->Write(bitstream);
		delete code;
	}
	v->IncWeightAndModify();
	return true;
}

template <class T> 
HuffmanItemType CHuffmanTree<T>::Decode(CBitstream * bitstream,T * ret)		
{
	CHuffmanItem<T> * p = Last;
	while( p!=NULL && p->ItemType == Node)
	{
		int8_t dir = (int8_t)bitstream->getBits(1);
		if(dir == 1)
		{
			p = p->Right;
		}
		else
		{
			p = p->Left;
		}
	}
	if(p == NULL)
	{
		fprintf(stderr,"CHuffmanTree: Decode: p == NULL\n");
		return ERROR;
	}
	if(p->ItemType == EOB)
	{
		p->IncWeightAndModify();
		*ret=0;
		return EOB;
	}
	if(p->ItemType == New)
	{
		T symbol = (T)bitstream->getBits(this->symbol_size);
		this->Encode(symbol);
		*ret = symbol;
		return New;
	}
	T s = p->Value;
	HuffmanItemType itype= p->ItemType;
	this->Encode(p->Value);
	*ret = s;
	return itype;
}

template <class T> 
CHuffmanCode * CHuffmanTree<T>::GetCode(CHuffmanItem<T> * symbol)
{
	CHuffmanCode * code = new CHuffmanCode();
	CHuffmanItem<T> * p = symbol->Parent;
	while(p != NULL)
	{
		code->Put(symbol == p->Right);
		symbol = p;
		p=symbol->Parent;
	}
	return code;
}

template <class T> 
CHuffmanItem<T> * CHuffmanTree<T>::FindLeafValue(T value)
{
	CHuffmanItem<T> * p  = this->Last->Prev;
	while(p && ((p->ItemType == Leaf && p->Value != value) || p->ItemType == EOB || p->ItemType == New || p->ItemType == Node ))
	{
		p=p->Prev;
	}
	if(p && p->Value == value && p->ItemType == Leaf)
	{
		return p;
	}
	return NULL;
}

template <class T> 
CHuffmanItem<T> * CHuffmanTree<T>::FindItemType(HuffmanItemType itemtype)
{
	if(itemtype == EOB)
	{
		CHuffmanItem<T> * p = this->First;
		while(p && p->ItemType != EOB)
		{
			p=p->Next;
		}
		if(p && p->ItemType==EOB)
		{
			return p;
		}
		return NULL;
	}
	if(itemtype == New)
	{
		CHuffmanItem<T> * p = this->Last->Prev;
		while(p && p->ItemType != New)
		{
			p=p->Prev;
		}
		if(p && p->ItemType==New)
		{
			return p;
		}
	}
	return NULL;
}

template <class T> 
void CHuffmanTree<T>::PrintAll() 
{
	Last->Print();
}

template <class T> 
CHuffmanItem<T> * CHuffmanItem<T>::FindN(uint32_t n)
{
	if(this->SN == n)
	{
		return this;
	}
	CHuffmanItem<T> * left = NULL;
	CHuffmanItem<T> * right = NULL;
	if(this->Right)
	{
		right = this->Right->FindN(n);
	}
	if(right)
	{
		return right;
	}
	if(this->Left)
	{
		left = this->Left->FindN(n);
	}
	if(left)
	{
		return left;
	}
	return NULL;
}

template <class T> 
void CHuffmanTree<T>::Leak()
{
	CHuffmanItem<T> * p = this->First;
	CHuffmanItem<T> * next = NULL;
	for(p = this->First;p;p=next)
	{
		next = p->Next;
		if(p->ItemType == Leaf)
		{
			p->DecWeightAndModify();
			if(First->Weight == 0)
			{
				this->leafCounter--;
				CHuffmanItem<T> * f = First;
				First = First->Next->Next;
				First->Prev = NULL;
				f->Next->Exchange(f->Parent);
				delete f->Next;
				delete f;
				next=First;
				this->FindItemType(New)->DecWeightAndModify();
			}
		}
	}
}

template <class T> 
uint32_t CHuffmanTree<T>::ItemCounter()
{
	return this->First->SN;
}

template <class T> 
uint32_t CHuffmanTree<T>::SymbolCounter()
{
	return this->leafCounter;
}

template <class T> 
uint64_t CHuffmanTree<T>::Counter()
{
	return this->Last->Weight;
}

template <class T> 
CHuffmanItem<T>::CHuffmanItem():
	Value(0),
	SN(0),
	Weight(1),
	Next(NULL),
	Prev(NULL),
	Left(NULL),
	Right(NULL),
	Parent(NULL),
	ItemType(None)
{}

template <class T> 
CHuffmanItem<T>::CHuffmanItem(const CHuffmanItem<T> & source)
{
	this->Value = source.Value;
	this->SN = source.SN;
	this->Weight = source.Weight;
	this->ItemType = source.ItemType;
	this->Next = NULL;
	this->Prev = NULL;
	if(source.ItemType == Node)
	{
		this->Left = new CHuffmanItem<T>(*source.Left);
		this->Right = new CHuffmanItem<T>(*source.Right);
		this->Left->Next = this->Right;
		this->Right->Prev = this->Left;
		this->Left->Parent = this;
		this->Right->Parent = this;
	}
	else
	{
		this->Left = NULL;
		this->Right = NULL;
	}
}

template <class T> 
CHuffmanItem<T>::CHuffmanItem(T value,uint64_t weight,CHuffmanItem * next,CHuffmanItem * parent,CHuffmanItem * left,CHuffmanItem * right,HuffmanItemType itemtype,CHuffmanItem * prev,uint32_t n) :
	Value(value),
	SN(n),
	Weight(weight),
	Next(next),
	Prev(prev),
	Left(left),
	Right(right),
	Parent(parent),
	ItemType(itemtype)
{
}

template <class T> 
CHuffmanItem<T>::~CHuffmanItem()
{	
}

template <class T> 
void CHuffmanItem<T>::IncWeightAndModify()
{
	this->Weight++;
	CHuffmanItem<T> * p = this;
	while(p->Next && p->Next->Weight == (this->Weight-1) )
	{
		p=p->Next;
	}
	if( p != this && p->Weight == (this->Weight-1) )
	{
		this->Exchange(p);
	}
	if(p->Parent) 
	{
		p->Parent->IncWeightAndModify();
	}
}

template <class T> 
void CHuffmanItem<T>::DecWeightAndModify()
{
	CHuffmanItem<T> * p = this;
	while(p->Prev && p->Prev->Weight == this->Weight) p=p->Prev;
	if(p != this && p->Weight == this->Weight)
	{
		this->Exchange(p);
	}
	p->Weight--;
	if(p->Parent)
	{
		p->Parent->DecWeightAndModify();
	}
}

template <class T> 
void CHuffmanItem<T>::DecWeight()
{
	this->Weight--;
	for(CHuffmanItem<T> * p = this->Parent; p ; p = p->Parent)
		p->Weight--;
}

template <class T> 
void CHuffmanItem<T>::Exchange(CHuffmanItem<T> * p,bool update_children)
{
	T value = p->Value;
	uint64_t weight = p->Weight;
	CHuffmanItem<T> * left = p->Left;
	CHuffmanItem<T> * right = p->Right;
	HuffmanItemType itemtype = p->ItemType;
	p->Value = this->Value;
	p->Weight = this->Weight;
	p->Left = this->Left;
	p->Right = this->Right;
	p->ItemType = this->ItemType;
	this->Value = value;
	this->Weight = weight;
	this->Left = left;
	this->Right = right;
	this->ItemType = itemtype;
	if(!update_children)
	{
		return;
	}
	if(p->Left) 
	{
		p->Left->Parent = p;
	}
	if(p->Right)
	{
		p->Right->Parent = p;
	}
	if(this->Left)
	{
		this->Left->Parent = this;
	}
	if(this->Right)
	{
		this->Right->Parent = this;
	}
}

template <class T> 
void CHuffmanItem<T>::Print()
{
	if(Left)
	{
		Left->Print();
	}
	if(Right)
	{
		Right->Print();
	}
	if(this->ItemType == Node)
	{
		//printf("[Node][%d]:Weight = %u val=%d \n",this->SN,(unsigned int)this->Weight,this->Value);
		return;
	}
	if(this->ItemType == New)
	{
		//printf("[New][%d]:Weight = %u val=%d \n",this->SN,(unsigned int)this->Weight,this->Value);
		return;
	}
	if(this->ItemType == EOB)
	{
		//printf("[EOB][%d]:Weight = %u val=%d\n",this->SN,(unsigned int)this->Weight,this->Value);
		return;
	}
	if(this->ItemType == Leaf)
	{
		//printf("[%d][%d]:Weight = %u itype=%d \n",this->Value,this->SN,(unsigned int)this->Weight,this->ItemType);
		return;
	}
}

INSTANTIATE(CHuffmanTree, uint8_t);
INSTANTIATE(CHuffmanTree, uint16_t);
INSTANTIATE(CHuffmanTree, int16_t);
INSTANTIATE(CHuffmanTree, uint32_t);
INSTANTIATE(CHuffmanTree, int32_t);
INSTANTIATE(CHuffmanTree, float);
}
