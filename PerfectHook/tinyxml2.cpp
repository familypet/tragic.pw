/*
Original code by Lee Thomason (www.grinninglizard.com)

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

#include "tinyxml2.h"

#include <new>		// yes, this one new style header, is in the Android SDK.
#if defined(ANDROID_NDK) || defined(__QNXNTO__)
#   include <stddef.h>
#   include <stdarg.h>
#else
#   include <cstddef>
#   include <cstdarg>
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1400 ) && (!defined WINCE)
// Microsoft Visual Studio, version 2005 and higher. Not WinCE.
/*int _snprintf_s(
   char *buffer,
   size_t sizeOfBuffer,
   size_t count,
   const char *format [,
	  argument] ...
);*/
static inline int TIXML_SNPRINTF(char* buffer, size_t size, const char* format, ...)
{
	va_list va;
	va_start( va, format );
	int result = vsnprintf_s(buffer, size, _TRUNCATE, format, va);
	va_end( va );
	return result;
}

static inline int TIXML_VSNPRINTF(char* buffer, size_t size, const char* format, va_list va)
{
	int result = vsnprintf_s(buffer, size, _TRUNCATE, format, va);
	return result;
}

#define TIXML_VSCPRINTF	_vscprintf
#define TIXML_SSCANF	sscanf_s
#elif defined _MSC_VER
// Microsoft Visual Studio 2003 and earlier or WinCE
	#define TIXML_SNPRINTF	_snprintf
	#define TIXML_VSNPRINTF _vsnprintf
	#define TIXML_SSCANF	sscanf
#if (_MSC_VER < 1400 ) && (!defined WINCE)
// Microsoft Visual Studio 2003 and not WinCE.
		#define TIXML_VSCPRINTF   _vscprintf // VS2003's C runtime has this, but VC6 C runtime or WinCE SDK doesn't have.
#else
// Microsoft Visual Studio 2003 and earlier or WinCE.
		static inline int TIXML_VSCPRINTF( const char* format, va_list va )
		{
			int len = 512;
			for (;;) {
				len = len*2;
				char* str = new char[len]();
				const int required = _vsnprintf(str, len, format, va);
				delete[] str;
				if ( required != -1 ) {
					TIXMLASSERT( required >= 0 );
					len = required;
					break;
				}
			}
			TIXMLASSERT( len >= 0 );
			return len;
		}
#endif
#else
// GCC version 3 and higher
//#warning( "Using sn* functions." )
	#define TIXML_SNPRINTF	snprintf
	#define TIXML_VSNPRINTF	vsnprintf
	static inline int TIXML_VSCPRINTF( const char* format, va_list va )
	{
		int len = vsnprintf( 0, 0, format, va );
		TIXMLASSERT( len >= 0 );
		return len;
	}
	#define TIXML_SSCANF   sscanf
#endif


static const char LINE_FEED = (char)0x0a; // all line endings are normalized to LF
static const char LF = LINE_FEED;
static const char CARRIAGE_RETURN = (char)0x0d; // CR gets filtered out
static const char CR = CARRIAGE_RETURN;
static const char SINGLE_QUOTE = '\'';
static const char DOUBLE_QUOTE = '\"';

// Bunch of unicode info at:
//		http://www.unicode.org/faq/utf_bom.html
//	ef bb bf (Microsoft "lead bytes") - designates UTF-8

static const unsigned char TIXML_UTF_LEAD_0 = 0xefU;
static const unsigned char TIXML_UTF_LEAD_1 = 0xbbU;
static const unsigned char TIXML_UTF_LEAD_2 = 0xbfU;

namespace tinyxml2
{
	struct Entity
	{
		const char* pattern;
		int length;
		char value;
	};

	static const int NUM_ENTITIES = 5;
	static const Entity entities[NUM_ENTITIES] = {
		{"quot", 4, DOUBLE_QUOTE},
		{"amp", 3, '&'},
		{"apos", 4, SINGLE_QUOTE},
		{"lt", 2, '<'},
		{"gt", 2, '>'}
	};


	StrPair::~StrPair()
	{
		Reset();
	}


	void StrPair::TransferTo(StrPair* other)
	{
		if (this == other)
		{
			return;
		}
		// This in effect implements the assignment operator by "moving"
		// ownership (as in auto_ptr).

		TIXMLASSERT( other->_flags == 0 );
		TIXMLASSERT( other->_start == 0 );
		TIXMLASSERT( other->_end == 0 );

		other->Reset();

		other->_flags = _flags;
		other->_start = _start;
		other->_end = _end;

		_flags = 0;
		_start = 0;
		_end = 0;
	}

	void StrPair::Reset()
	{
		if (_flags & NEEDS_DELETE)
		{
			delete [] _start;
		}
		_flags = 0;
		_start = 0;
		_end = 0;
	}


	void StrPair::SetStr(const char* str, int flags)
	{
		Reset();
		size_t len = strlen(str);
		TIXMLASSERT( _start == 0 );
		_start = new char[ len + 1 ];
		memcpy(_start, str, len + 1);
		_end = _start + len;
		_flags = flags | NEEDS_DELETE;
	}


	char* StrPair::ParseText(char* p, const char* endTag, int strFlags)
	{
		TIXMLASSERT( endTag && *endTag );

		char* start = p;
		char endChar = *endTag;
		size_t length = strlen(endTag);

		// Inner loop of text parsing.
		while (*p)
		{
			if (*p == endChar && strncmp(p, endTag, length) == 0)
			{
				Set(start, p, strFlags);
				return p + length;
			}
			++p;
		}
		return 0;
	}


	char* StrPair::ParseName(char* p)
	{
		if (!p || !(*p))
		{
			return 0;
		}
		if (!XMLUtil::IsNameStartChar(*p))
		{
			return 0;
		}

		char* const start = p;
		++p;
		while (*p && XMLUtil::IsNameChar(*p))
		{
			++p;
		}

		Set(start, p, 0);
		return p;
	}


	void StrPair::CollapseWhitespace()
	{
		// Adjusting _start would cause undefined behavior on delete[]
		TIXMLASSERT( ( _flags & NEEDS_DELETE ) == 0 );
		// Trim leading space.
		_start = XMLUtil::SkipWhiteSpace(_start);

		if (*_start)
		{
			char* p = _start; // the read pointer
			char* q = _start; // the write pointer

			while (*p)
			{
				if (XMLUtil::IsWhiteSpace(*p))
				{
					p = XMLUtil::SkipWhiteSpace(p);
					if (*p == 0)
					{
						break; // don't write to q; this trims the trailing space.
					}
					*q = ' ';
					++q;
				}
				*q = *p;
				++q;
				++p;
			}
			*q = 0;
		}
	}


	const char* StrPair::GetStr()
	{
		TIXMLASSERT( _start );
		TIXMLASSERT( _end );
		if (_flags & NEEDS_FLUSH)
		{
			*_end = 0;
			_flags ^= NEEDS_FLUSH;

			if (_flags)
			{
				char* p = _start; // the read pointer
				char* q = _start; // the write pointer

				while (p < _end)
				{
					if ((_flags & NEEDS_NEWLINE_NORMALIZATION) && *p == CR)
					{
						// CR-LF pair becomes LF
						// CR alone becomes LF
						// LF-CR becomes LF
						if (*(p + 1) == LF)
						{
							p += 2;
						}
						else
						{
							++p;
						}
						*q++ = LF;
					}
					else if ((_flags & NEEDS_NEWLINE_NORMALIZATION) && *p == LF)
					{
						if (*(p + 1) == CR)
						{
							p += 2;
						}
						else
						{
							++p;
						}
						*q++ = LF;
					}
					else if ((_flags & NEEDS_ENTITY_PROCESSING) && *p == '&')
					{
						// Entities handled by tinyXML2:
						// - special entities in the entity table [in/out]
						// - numeric character reference [in]
						//   &#20013; or &#x4e2d;

						if (*(p + 1) == '#')
						{
							const int buflen = 10;
							char buf[buflen] = {0};
							int len = 0;
							char* adjusted = const_cast<char*>(XMLUtil::GetCharacterRef(p, buf, &len));
							if (adjusted == 0)
							{
								*q = *p;
								++p;
								++q;
							}
							else
							{
								TIXMLASSERT( 0 <= len && len <= buflen );
								TIXMLASSERT( q + len <= adjusted );
								p = adjusted;
								memcpy(q, buf, len);
								q += len;
							}
						}
						else
						{
							bool entityFound = false;
							for (int i = 0; i < NUM_ENTITIES; ++i)
							{
								const Entity& entity = entities[i];
								if (strncmp(p + 1, entity.pattern, entity.length) == 0
									&& *(p + entity.length + 1) == ';')
								{
									// Found an entity - convert.
									*q = entity.value;
									++q;
									p += entity.length + 2;
									entityFound = true;
									break;
								}
							}
							if (!entityFound)
							{
								// fixme: treat as error?
								++p;
								++q;
							}
						}
					}
					else
					{
						*q = *p;
						++p;
						++q;
					}
				}
				*q = 0;
			}
			// The loop below has plenty going on, and this
			// is a less useful mode. Break it out.
			if (_flags & NEEDS_WHITESPACE_COLLAPSING)
			{
				CollapseWhitespace();
			}
			_flags = (_flags & NEEDS_DELETE);
		}
		TIXMLASSERT( _start );
		return _start;
	}


	// --------- XMLUtil ----------- //

	const char* XMLUtil::ReadBOM(const char* p, bool* bom)
	{
		TIXMLASSERT( p );
		TIXMLASSERT( bom );
		*bom = false;
		const unsigned char* pu = reinterpret_cast<const unsigned char*>(p);
		// Check for BOM:
		if (*(pu + 0) == TIXML_UTF_LEAD_0
			&& *(pu + 1) == TIXML_UTF_LEAD_1
			&& *(pu + 2) == TIXML_UTF_LEAD_2)
		{
			*bom = true;
			p += 3;
		}
		TIXMLASSERT( p );
		return p;
	}


	void XMLUtil::ConvertUTF32ToUTF8(unsigned long input, char* output, int* length)
	{
		const unsigned long BYTE_MASK = 0xBF;
		const unsigned long BYTE_MARK = 0x80;
		const unsigned long FIRST_BYTE_MARK[7] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};

		if (input < 0x80)
		{
			*length = 1;
		}
		else if (input < 0x800)
		{
			*length = 2;
		}
		else if (input < 0x10000)
		{
			*length = 3;
		}
		else if (input < 0x200000)
		{
			*length = 4;
		}
		else
		{
			*length = 0; // This code won't convert this correctly anyway.
			return;
		}

		output += *length;

		// Scary scary fall throughs.
		switch (*length)
		{
		case 4:
			--output;
			*output = (char)((input | BYTE_MARK) & BYTE_MASK);
			input >>= 6;
		case 3:
			--output;
			*output = (char)((input | BYTE_MARK) & BYTE_MASK);
			input >>= 6;
		case 2:
			--output;
			*output = (char)((input | BYTE_MARK) & BYTE_MASK);
			input >>= 6;
		case 1:
			--output;
			*output = (char)(input | FIRST_BYTE_MARK[*length]);
			break;
		default:
			TIXMLASSERT( false );
		}
	}


	const char* XMLUtil::GetCharacterRef(const char* p, char* value, int* length)
	{
		// Presume an entity, and pull it out.
		*length = 0;

		if (*(p + 1) == '#' && *(p + 2))
		{
			unsigned long ucs = 0;
			TIXMLASSERT( sizeof( ucs ) >= 4 );
			ptrdiff_t delta = 0;
			unsigned mult = 1;
			static const char SEMICOLON = ';';

			if (*(p + 2) == 'x')
			{
				// Hexadecimal.
				const char* q = p + 3;
				if (!(*q))
				{
					return 0;
				}

				q = strchr(q, SEMICOLON);

				if (!q)
				{
					return 0;
				}
				TIXMLASSERT( *q == SEMICOLON );

				delta = q - p;
				--q;

				while (*q != 'x')
				{
					unsigned int digit = 0;

					if (*q >= '0' && *q <= '9')
					{
						digit = *q - '0';
					}
					else if (*q >= 'a' && *q <= 'f')
					{
						digit = *q - 'a' + 10;
					}
					else if (*q >= 'A' && *q <= 'F')
					{
						digit = *q - 'A' + 10;
					}
					else
					{
						return 0;
					}
					TIXMLASSERT( digit >= 0 && digit < 16);
					TIXMLASSERT( digit == 0 || mult <= UINT_MAX / digit );
					const unsigned int digitScaled = mult * digit;
					TIXMLASSERT( ucs <= ULONG_MAX - digitScaled );
					ucs += digitScaled;
					TIXMLASSERT( mult <= UINT_MAX / 16 );
					mult *= 16;
					--q;
				}
			}
			else
			{
				// Decimal.
				const char* q = p + 2;
				if (!(*q))
				{
					return 0;
				}

				q = strchr(q, SEMICOLON);

				if (!q)
				{
					return 0;
				}
				TIXMLASSERT( *q == SEMICOLON );

				delta = q - p;
				--q;

				while (*q != '#')
				{
					if (*q >= '0' && *q <= '9')
					{
						const unsigned int digit = *q - '0';
						TIXMLASSERT( digit >= 0 && digit < 10);
						TIXMLASSERT( digit == 0 || mult <= UINT_MAX / digit );
						const unsigned int digitScaled = mult * digit;
						TIXMLASSERT( ucs <= ULONG_MAX - digitScaled );
						ucs += digitScaled;
					}
					else
					{
						return 0;
					}
					TIXMLASSERT( mult <= UINT_MAX / 10 );
					mult *= 10;
					--q;
				}
			}
			// convert the UCS to UTF-8
			ConvertUTF32ToUTF8(ucs, value, length);
			return p + delta + 1;
		}
		return p + 1;
	}


	void XMLUtil::ToStr(int v, char* buffer, int bufferSize)
	{
		TIXML_SNPRINTF(buffer, bufferSize, "%d", v);
	}


	void XMLUtil::ToStr(unsigned v, char* buffer, int bufferSize)
	{
		TIXML_SNPRINTF(buffer, bufferSize, "%u", v);
	}


	void XMLUtil::ToStr(bool v, char* buffer, int bufferSize)
	{
		TIXML_SNPRINTF(buffer, bufferSize, "%d", v ? 1 : 0);
	}

	/*
		ToStr() of a number is a very tricky topic.
		https://github.com/leethomason/tinyxml2/issues/106
	*/
	void XMLUtil::ToStr(float v, char* buffer, int bufferSize)
	{
		TIXML_SNPRINTF(buffer, bufferSize, "%.8g", v);
	}


	void XMLUtil::ToStr(double v, char* buffer, int bufferSize)
	{
		TIXML_SNPRINTF(buffer, bufferSize, "%.17g", v);
	}


	bool XMLUtil::ToInt(const char* str, int* value)
	{
		if (TIXML_SSCANF(str, "%d", value) == 1)
		{
			return true;
		}
		return false;
	}

	bool XMLUtil::ToUnsigned(const char* str, unsigned* value)
	{
		if (TIXML_SSCANF(str, "%u", value) == 1)
		{
			return true;
		}
		return false;
	}

	bool XMLUtil::ToBool(const char* str, bool* value)
	{
		int ival = 0;
		if (ToInt(str, &ival))
		{
			*value = (ival == 0) ? false : true;
			return true;
		}
		if (StringEqual(str, "true"))
		{
			*value = true;
			return true;
		}
		else if (StringEqual(str, "false"))
		{
			*value = false;
			return true;
		}
		return false;
	}


	bool XMLUtil::ToFloat(const char* str, float* value)
	{
		if (TIXML_SSCANF(str, "%f", value) == 1)
		{
			return true;
		}
		return false;
	}

	bool XMLUtil::ToDouble(const char* str, double* value)
	{
		if (TIXML_SSCANF(str, "%lf", value) == 1)
		{
			return true;
		}
		return false;
	}


	char* XMLDocument::Identify(char* p, XMLNode** node)
	{
		TIXMLASSERT( node );
		TIXMLASSERT( p );
		char* const start = p;
		p = XMLUtil::SkipWhiteSpace(p);
		if (!*p)
		{
			*node = 0;
			TIXMLASSERT( p );
			return p;
		}

		// These strings define the matching patterns:
		static const char* xmlHeader = {"<?"};
		static const char* commentHeader = {"<!--"};
		static const char* cdataHeader = {"<![CDATA["};
		static const char* dtdHeader = {"<!"};
		static const char* elementHeader = {"<"}; // and a header for everything else; check last.

		static const int xmlHeaderLen = 2;
		static const int commentHeaderLen = 4;
		static const int cdataHeaderLen = 9;
		static const int dtdHeaderLen = 2;
		static const int elementHeaderLen = 1;

		TIXMLASSERT( sizeof( XMLComment ) == sizeof( XMLUnknown ) ); // use same memory pool
		TIXMLASSERT( sizeof( XMLComment ) == sizeof( XMLDeclaration ) ); // use same memory pool
		XMLNode* returnNode = 0;
		if (XMLUtil::StringEqual(p, xmlHeader, xmlHeaderLen))
		{
			TIXMLASSERT( sizeof( XMLDeclaration ) == _commentPool.ItemSize() );
			returnNode = new(_commentPool.Alloc()) XMLDeclaration(this);
			returnNode->_memPool = &_commentPool;
			p += xmlHeaderLen;
		}
		else if (XMLUtil::StringEqual(p, commentHeader, commentHeaderLen))
		{
			TIXMLASSERT( sizeof( XMLComment ) == _commentPool.ItemSize() );
			returnNode = new(_commentPool.Alloc()) XMLComment(this);
			returnNode->_memPool = &_commentPool;
			p += commentHeaderLen;
		}
		else if (XMLUtil::StringEqual(p, cdataHeader, cdataHeaderLen))
		{
			TIXMLASSERT( sizeof( XMLText ) == _textPool.ItemSize() );
			XMLText* text = new(_textPool.Alloc()) XMLText(this);
			returnNode = text;
			returnNode->_memPool = &_textPool;
			p += cdataHeaderLen;
			text->SetCData(true);
		}
		else if (XMLUtil::StringEqual(p, dtdHeader, dtdHeaderLen))
		{
			TIXMLASSERT( sizeof( XMLUnknown ) == _commentPool.ItemSize() );
			returnNode = new(_commentPool.Alloc()) XMLUnknown(this);
			returnNode->_memPool = &_commentPool;
			p += dtdHeaderLen;
		}
		else if (XMLUtil::StringEqual(p, elementHeader, elementHeaderLen))
		{
			TIXMLASSERT( sizeof( XMLElement ) == _elementPool.ItemSize() );
			returnNode = new(_elementPool.Alloc()) XMLElement(this);
			returnNode->_memPool = &_elementPool;
			p += elementHeaderLen;
		}
		else
		{
			TIXMLASSERT( sizeof( XMLText ) == _textPool.ItemSize() );
			returnNode = new(_textPool.Alloc()) XMLText(this);
			returnNode->_memPool = &_textPool;
			p = start; // Back it up, all the text counts.
		}

		TIXMLASSERT( returnNode );
		TIXMLASSERT( p );
		*node = returnNode;
		return p;
	}


	bool XMLDocument::Accept(XMLVisitor* visitor) const
	{
		TIXMLASSERT( visitor );
		if (visitor->VisitEnter(*this))
		{
			for (const XMLNode* node = FirstChild(); node; node = node->NextSibling())
			{
				if (!node->Accept(visitor))
				{
					break;
				}
			}
		}
		return visitor->VisitExit(*this);
	}


	// --------- XMLNode ----------- //

	XMLNode::XMLNode(XMLDocument* doc) :
		_document(doc),
		_parent(0),
		_firstChild(0), _lastChild(0),
		_prev(0), _next(0),
		_memPool(0)
	{
	}


	XMLNode::~XMLNode()
	{
		DeleteChildren();
		if (_parent)
		{
			_parent->Unlink(this);
		}
	}

	const char* XMLNode::Value() const
	{
		// Catch an edge case: XMLDocuments don't have a a Value. Carefully return nullptr.
		if (this->ToDocument())
			return 0;
		return _value.GetStr();
	}

	void XMLNode::SetValue(const char* str, bool staticMem)
	{
		if (staticMem)
		{
			_value.SetInternedStr(str);
		}
		else
		{
			_value.SetStr(str);
		}
	}


	void XMLNode::DeleteChildren()
	{
		while (_firstChild)
		{
			TIXMLASSERT( _lastChild );
			TIXMLASSERT( _firstChild->_document == _document );
			XMLNode* node = _firstChild;
			Unlink(node);

			DeleteNode(node);
		}
		_firstChild = _lastChild = 0;
	}


	void XMLNode::Unlink(XMLNode* child)
	{
		TIXMLASSERT( child );
		TIXMLASSERT( child->_document == _document );
		TIXMLASSERT( child->_parent == this );
		if (child == _firstChild)
		{
			_firstChild = _firstChild->_next;
		}
		if (child == _lastChild)
		{
			_lastChild = _lastChild->_prev;
		}

		if (child->_prev)
		{
			child->_prev->_next = child->_next;
		}
		if (child->_next)
		{
			child->_next->_prev = child->_prev;
		}
		child->_parent = 0;
	}


	void XMLNode::DeleteChild(XMLNode* node)
	{
		TIXMLASSERT( node );
		TIXMLASSERT( node->_document == _document );
		TIXMLASSERT( node->_parent == this );
		DeleteNode(node);
	}


	XMLNode* XMLNode::InsertEndChild(XMLNode* addThis)
	{
		TIXMLASSERT( addThis );
		if (addThis->_document != _document)
		{
			TIXMLASSERT( false );
			return 0;
		}
		InsertChildPreamble(addThis);

		if (_lastChild)
		{
			TIXMLASSERT( _firstChild );
			TIXMLASSERT( _lastChild->_next == 0 );
			_lastChild->_next = addThis;
			addThis->_prev = _lastChild;
			_lastChild = addThis;

			addThis->_next = 0;
		}
		else
		{
			TIXMLASSERT( _firstChild == 0 );
			_firstChild = _lastChild = addThis;

			addThis->_prev = 0;
			addThis->_next = 0;
		}
		addThis->_parent = this;
		return addThis;
	}


	XMLNode* XMLNode::InsertFirstChild(XMLNode* addThis)
	{
		TIXMLASSERT( addThis );
		if (addThis->_document != _document)
		{
			TIXMLASSERT( false );
			return 0;
		}
		InsertChildPreamble(addThis);

		if (_firstChild)
		{
			TIXMLASSERT( _lastChild );
			TIXMLASSERT( _firstChild->_prev == 0 );

			_firstChild->_prev = addThis;
			addThis->_next = _firstChild;
			_firstChild = addThis;

			addThis->_prev = 0;
		}
		else
		{
			TIXMLASSERT( _lastChild == 0 );
			_firstChild = _lastChild = addThis;

			addThis->_prev = 0;
			addThis->_next = 0;
		}
		addThis->_parent = this;
		return addThis;
	}


	XMLNode* XMLNode::InsertAfterChild(XMLNode* afterThis, XMLNode* addThis)
	{
		TIXMLASSERT( addThis );
		if (addThis->_document != _document)
		{
			TIXMLASSERT( false );
			return 0;
		}

		TIXMLASSERT( afterThis );

		if (afterThis->_parent != this)
		{
			TIXMLASSERT( false );
			return 0;
		}

		if (afterThis->_next == 0)
		{
			// The last node or the only node.
			return InsertEndChild(addThis);
		}
		InsertChildPreamble(addThis);
		addThis->_prev = afterThis;
		addThis->_next = afterThis->_next;
		afterThis->_next->_prev = addThis;
		afterThis->_next = addThis;
		addThis->_parent = this;
		return addThis;
	}


	const XMLElement* XMLNode::FirstChildElement(const char* name) const
	{
		for (const XMLNode* node = _firstChild; node; node = node->_next)
		{
			const XMLElement* element = node->ToElement();
			if (element)
			{
				if (!name || XMLUtil::StringEqual(element->Name(), name))
				{
					return element;
				}
			}
		}
		return 0;
	}


	const XMLElement* XMLNode::LastChildElement(const char* name) const
	{
		for (const XMLNode* node = _lastChild; node; node = node->_prev)
		{
			const XMLElement* element = node->ToElement();
			if (element)
			{
				if (!name || XMLUtil::StringEqual(element->Name(), name))
				{
					return element;
				}
			}
		}
		return 0;
	}


	const XMLElement* XMLNode::NextSiblingElement(const char* name) const
	{
		for (const XMLNode* node = _next; node; node = node->_next)
		{
			const XMLElement* element = node->ToElement();
			if (element
				&& (!name || XMLUtil::StringEqual(name, element->Name())))
			{
				return element;
			}
		}
		return 0;
	}


	const XMLElement* XMLNode::PreviousSiblingElement(const char* name) const
	{
		for (const XMLNode* node = _prev; node; node = node->_prev)
		{
			const XMLElement* element = node->ToElement();
			if (element
				&& (!name || XMLUtil::StringEqual(name, element->Name())))
			{
				return element;
			}
		}
		return 0;
	}


	char* XMLNode::ParseDeep(char* p, StrPair* parentEnd)
	{
		// This is a recursive method, but thinking about it "at the current level"
		// it is a pretty simple flat list:
		//		<foo/>
		//		<!-- comment -->
		//
		// With a special case:
		//		<foo>
		//		</foo>
		//		<!-- comment -->
		//
		// Where the closing element (/foo) *must* be the next thing after the opening
		// element, and the names must match. BUT the tricky bit is that the closing
		// element will be read by the child.
		//
		// 'endTag' is the end tag for this node, it is returned by a call to a child.
		// 'parentEnd' is the end tag for the parent, which is filled in and returned.

		while (p && *p)
		{
			XMLNode* node = 0;

			p = _document->Identify(p, &node);
			if (node == 0)
			{
				break;
			}

			StrPair endTag;
			p = node->ParseDeep(p, &endTag);
			if (!p)
			{
				DeleteNode(node);
				if (!_document->Error())
				{
					_document->SetError(XML_ERROR_PARSING, 0, 0);
				}
				break;
			}

			XMLDeclaration* decl = node->ToDeclaration();
			if (decl)
			{
				// A declaration can only be the first child of a document.
				// Set error, if document already has children.
				if (!_document->NoChildren())
				{
					_document->SetError(XML_ERROR_PARSING_DECLARATION, decl->Value(), 0);
					DeleteNode(decl);
					break;
				}
			}

			XMLElement* ele = node->ToElement();
			if (ele)
			{
				// We read the end tag. Return it to the parent.
				if (ele->ClosingType() == XMLElement::CLOSING)
				{
					if (parentEnd)
					{
						ele->_value.TransferTo(parentEnd);
					}
					node->_memPool->SetTracked(); // created and then immediately deleted.
					DeleteNode(node);
					return p;
				}

				// Handle an end tag returned to this level.
				// And handle a bunch of annoying errors.
				bool mismatch = false;
				if (endTag.Empty())
				{
					if (ele->ClosingType() == XMLElement::OPEN)
					{
						mismatch = true;
					}
				}
				else
				{
					if (ele->ClosingType() != XMLElement::OPEN)
					{
						mismatch = true;
					}
					else if (!XMLUtil::StringEqual(endTag.GetStr(), ele->Name()))
					{
						mismatch = true;
					}
				}
				if (mismatch)
				{
					_document->SetError(XML_ERROR_MISMATCHED_ELEMENT, ele->Name(), 0);
					DeleteNode(node);
					break;
				}
			}
			InsertEndChild(node);
		}
		return 0;
	}

	void XMLNode::DeleteNode(XMLNode* node)
	{
		if (node == 0)
		{
			return;
		}
		MemPool* pool = node->_memPool;
		node->~XMLNode();
		pool->Free(node);
	}

	void XMLNode::InsertChildPreamble(XMLNode* insertThis) const
	{
		TIXMLASSERT( insertThis );
		TIXMLASSERT( insertThis->_document == _document );

		if (insertThis->_parent)
			insertThis->_parent->Unlink(insertThis);
		else
			insertThis->_memPool->SetTracked();
	}

	// --------- XMLText ---------- //
	char* XMLText::ParseDeep(char* p, StrPair*)
	{
		const char* start = p;
		if (this->CData())
		{
			p = _value.ParseText(p, "]]>", StrPair::NEEDS_NEWLINE_NORMALIZATION);
			if (!p)
			{
				_document->SetError(XML_ERROR_PARSING_CDATA, start, 0);
			}
			return p;
		}
		else
		{
			int flags = _document->ProcessEntities() ? StrPair::TEXT_ELEMENT : StrPair::TEXT_ELEMENT_LEAVE_ENTITIES;
			if (_document->WhitespaceMode() == COLLAPSE_WHITESPACE)
			{
				flags |= StrPair::NEEDS_WHITESPACE_COLLAPSING;
			}

			p = _value.ParseText(p, "<", flags);
			if (p && *p)
			{
				return p - 1;
			}
			if (!p)
			{
				_document->SetError(XML_ERROR_PARSING_TEXT, start, 0);
			}
		}
		return 0;
	}


	XMLNode* XMLText::ShallowClone(XMLDocument* doc) const
	{
		if (!doc)
		{
			doc = _document;
		}
		XMLText* text = doc->NewText(Value()); // fixme: this will always allocate memory. Intern?
		text->SetCData(this->CData());
		return text;
	}


	bool XMLText::ShallowEqual(const XMLNode* compare) const
	{
		const XMLText* text = compare->ToText();
		return (text && XMLUtil::StringEqual(text->Value(), Value()));
	}


	bool XMLText::Accept(XMLVisitor* visitor) const
	{
		TIXMLASSERT( visitor );
		return visitor->Visit(*this);
	}


	// --------- XMLComment ---------- //

	XMLComment::XMLComment(XMLDocument* doc) : XMLNode(doc)
	{
	}


	XMLComment::~XMLComment()
	{
	}


	char* XMLComment::ParseDeep(char* p, StrPair*)
	{
		// Comment parses as text.
		const char* start = p;
		p = _value.ParseText(p, "-->", StrPair::COMMENT);
		if (p == 0)
		{
			_document->SetError(XML_ERROR_PARSING_COMMENT, start, 0);
		}
		return p;
	}


	XMLNode* XMLComment::ShallowClone(XMLDocument* doc) const
	{
		if (!doc)
		{
			doc = _document;
		}
		XMLComment* comment = doc->NewComment(Value()); // fixme: this will always allocate memory. Intern?
		return comment;
	}


	bool XMLComment::ShallowEqual(const XMLNode* compare) const
	{
		TIXMLASSERT( compare );
		const XMLComment* comment = compare->ToComment();
		return (comment && XMLUtil::StringEqual(comment->Value(), Value()));
	}


	bool XMLComment::Accept(XMLVisitor* visitor) const
	{
		TIXMLASSERT( visitor );
		return visitor->Visit(*this);
	}


	// --------- XMLDeclaration ---------- //

	XMLDeclaration::XMLDeclaration(XMLDocument* doc) : XMLNode(doc)
	{
	}


	XMLDeclaration::~XMLDeclaration()
	{
		//printf( "~XMLDeclaration\n" );
	}


	char* XMLDeclaration::ParseDeep(char* p, StrPair*)
	{
		// Declaration parses as text.
		const char* start = p;
		p = _value.ParseText(p, "?>", StrPair::NEEDS_NEWLINE_NORMALIZATION);
		if (p == 0)
		{
			_document->SetError(XML_ERROR_PARSING_DECLARATION, start, 0);
		}
		return p;
	}


	XMLNode* XMLDeclaration::ShallowClone(XMLDocument* doc) const
	{
		if (!doc)
		{
			doc = _document;
		}
		XMLDeclaration* dec = doc->NewDeclaration(Value()); // fixme: this will always allocate memory. Intern?
		return dec;
	}


	bool XMLDeclaration::ShallowEqual(const XMLNode* compare) const
	{
		TIXMLASSERT( compare );
		const XMLDeclaration* declaration = compare->ToDeclaration();
		return (declaration && XMLUtil::StringEqual(declaration->Value(), Value()));
	}


	bool XMLDeclaration::Accept(XMLVisitor* visitor) const
	{
		TIXMLASSERT( visitor );
		return visitor->Visit(*this);
	}

	// --------- XMLUnknown ---------- //

	XMLUnknown::XMLUnknown(XMLDocument* doc) : XMLNode(doc)
	{
	}


	XMLUnknown::~XMLUnknown()
	{
	}


	char* XMLUnknown::ParseDeep(char* p, StrPair*)
	{
		// Unknown parses as text.
		const char* start = p;

		p = _value.ParseText(p, ">", StrPair::NEEDS_NEWLINE_NORMALIZATION);
		if (!p)
		{
			_document->SetError(XML_ERROR_PARSING_UNKNOWN, start, 0);
		}
		return p;
	}


	XMLNode* XMLUnknown::ShallowClone(XMLDocument* doc) const
	{
		if (!doc)
		{
			doc = _document;
		}
		XMLUnknown* text = doc->NewUnknown(Value()); // fixme: this will always allocate memory. Intern?
		return text;
	}


	bool XMLUnknown::ShallowEqual(const XMLNode* compare) const
	{
		TIXMLASSERT( compare );
		const XMLUnknown* unknown = compare->ToUnknown();
		return (unknown && XMLUtil::StringEqual(unknown->Value(), Value()));
	}


	bool XMLUnknown::Accept(XMLVisitor* visitor) const
	{
		TIXMLASSERT( visitor );
		return visitor->Visit(*this);
	}

	// --------- XMLAttribute ---------- //

	const char* XMLAttribute::Name() const
	{
		return _name.GetStr();
	}

	const char* XMLAttribute::Value() const
	{
		return _value.GetStr();
	}

	char* XMLAttribute::ParseDeep(char* p, bool processEntities)
	{
		// Parse using the name rules: bug fix, was using ParseText before
		p = _name.ParseName(p);
		if (!p || !*p)
		{
			return 0;
		}

		// Skip white space before =
		p = XMLUtil::SkipWhiteSpace(p);
		if (*p != '=')
		{
			return 0;
		}

		++p; // move up to opening quote
		p = XMLUtil::SkipWhiteSpace(p);
		if (*p != '\"' && *p != '\'')
		{
			return 0;
		}

		char endTag[2] = {*p, 0};
		++p; // move past opening quote

		p = _value.ParseText(p, endTag, processEntities ? StrPair::ATTRIBUTE_VALUE : StrPair::ATTRIBUTE_VALUE_LEAVE_ENTITIES);
		return p;
	}


	void XMLAttribute::SetName(const char* n)
	{
		_name.SetStr(n);
	}


	XMLError XMLAttribute::QueryIntValue(int* value) const
	{
		if (XMLUtil::ToInt(Value(), value))
		{
			return XML_NO_ERROR;
		}
		return XML_WRONG_ATTRIBUTE_TYPE;
	}


	XMLError XMLAttribute::QueryUnsignedValue(unsigned int* value) const
	{
		if (XMLUtil::ToUnsigned(Value(), value))
		{
			return XML_NO_ERROR;
		}
		return XML_WRONG_ATTRIBUTE_TYPE;
	}


	XMLError XMLAttribute::QueryBoolValue(bool* value) const
	{
		if (XMLUtil::ToBool(Value(), value))
		{
			return XML_NO_ERROR;
		}
		return XML_WRONG_ATTRIBUTE_TYPE;
	}


	XMLError XMLAttribute::QueryFloatValue(float* value) const
	{
		if (XMLUtil::ToFloat(Value(), value))
		{
			return XML_NO_ERROR;
		}
		return XML_WRONG_ATTRIBUTE_TYPE;
	}


	XMLError XMLAttribute::QueryDoubleValue(double* value) const
	{
		if (XMLUtil::ToDouble(Value(), value))
		{
			return XML_NO_ERROR;
		}
		return XML_WRONG_ATTRIBUTE_TYPE;
	}


	void XMLAttribute::SetAttribute(const char* v)
	{
		_value.SetStr(v);
	}


	void XMLAttribute::SetAttribute(int v)
	{
		char buf[BUF_SIZE];
		XMLUtil::ToStr(v, buf, BUF_SIZE);
		_value.SetStr(buf);
	}


	void XMLAttribute::SetAttribute(unsigned v)
	{
		char buf[BUF_SIZE];
		XMLUtil::ToStr(v, buf, BUF_SIZE);
		_value.SetStr(buf);
	}


	void XMLAttribute::SetAttribute(bool v)
	{
		char buf[BUF_SIZE];
		XMLUtil::ToStr(v, buf, BUF_SIZE);
		_value.SetStr(buf);
	}

	void XMLAttribute::SetAttribute(double v)
	{
		char buf[BUF_SIZE];
		XMLUtil::ToStr(v, buf, BUF_SIZE);
		_value.SetStr(buf);
	}

	void XMLAttribute::SetAttribute(float v)
	{
		char buf[BUF_SIZE];
		XMLUtil::ToStr(v, buf, BUF_SIZE);
		_value.SetStr(buf);
	}


	// --------- XMLElement ---------- //
	XMLElement::XMLElement(XMLDocument* doc) : XMLNode(doc),
	                                           _closingType(0),
	                                           _rootAttribute(0)
	{
	}


	XMLElement::~XMLElement()
	{
		while (_rootAttribute)
		{
			XMLAttribute* next = _rootAttribute->_next;
			DeleteAttribute(_rootAttribute);
			_rootAttribute = next;
		}
	}


	const XMLAttribute* XMLElement::FindAttribute(const char* name) const
	{
		for (XMLAttribute* a = _rootAttribute; a; a = a->_next)
		{
			if (XMLUtil::StringEqual(a->Name(), name))
			{
				return a;
			}
		}
		return 0;
	}


	const char* XMLElement::Attribute(const char* name, const char* value) const
	{
		const XMLAttribute* a = FindAttribute(name);
		if (!a)
		{
			return 0;
		}
		if (!value || XMLUtil::StringEqual(a->Value(), value))
		{
			return a->Value();
		}
		return 0;
	}


	const char* XMLElement::GetText() const
	{
		if (FirstChild() && FirstChild()->ToText())
		{
			return FirstChild()->Value();
		}
		return 0;
	}


	void XMLElement::SetText(const char* inText)
	{
		if (FirstChild() && FirstChild()->ToText())
			FirstChild()->SetValue(inText);
		else
		{
			XMLText* theText = GetDocument()->NewText(inText);
			InsertFirstChild(theText);
		}
	}


	void XMLElement::SetText(int v)
	{
		char buf[BUF_SIZE];
		XMLUtil::ToStr(v, buf, BUF_SIZE);
		SetText(buf);
	}


	void XMLElement::SetText(unsigned v)
	{
		char buf[BUF_SIZE];
		XMLUtil::ToStr(v, buf, BUF_SIZE);
		SetText(buf);
	}


	void XMLElement::SetText(bool v)
	{
		char buf[BUF_SIZE];
		XMLUtil::ToStr(v, buf, BUF_SIZE);
		SetText(buf);
	}


	void XMLElement::SetText(float v)
	{
		char buf[BUF_SIZE];
		XMLUtil::ToStr(v, buf, BUF_SIZE);
		SetText(buf);
	}


	void XMLElement::SetText(double v)
	{
		char buf[BUF_SIZE];
		XMLUtil::ToStr(v, buf, BUF_SIZE);
		SetText(buf);
	}


	XMLError XMLElement::QueryIntText(int* ival) const
	{
		if (FirstChild() && FirstChild()->ToText())
		{
			const char* t = FirstChild()->Value();
			if (XMLUtil::ToInt(t, ival))
			{
				return XML_SUCCESS;
			}
			return XML_CAN_NOT_CONVERT_TEXT;
		}
		return XML_NO_TEXT_NODE;
	}


	XMLError XMLElement::QueryUnsignedText(unsigned* uval) const
	{
		if (FirstChild() && FirstChild()->ToText())
		{
			const char* t = FirstChild()->Value();
			if (XMLUtil::ToUnsigned(t, uval))
			{
				return XML_SUCCESS;
			}
			return XML_CAN_NOT_CONVERT_TEXT;
		}
		return XML_NO_TEXT_NODE;
	}


	XMLError XMLElement::QueryBoolText(bool* bval) const
	{
		if (FirstChild() && FirstChild()->ToText())
		{
			const char* t = FirstChild()->Value();
			if (XMLUtil::ToBool(t, bval))
			{
				return XML_SUCCESS;
			}
			return XML_CAN_NOT_CONVERT_TEXT;
		}
		return XML_NO_TEXT_NODE;
	}


	XMLError XMLElement::QueryDoubleText(double* dval) const
	{
		if (FirstChild() && FirstChild()->ToText())
		{
			const char* t = FirstChild()->Value();
			if (XMLUtil::ToDouble(t, dval))
			{
				return XML_SUCCESS;
			}
			return XML_CAN_NOT_CONVERT_TEXT;
		}
		return XML_NO_TEXT_NODE;
	}


	XMLError XMLElement::QueryFloatText(float* fval) const
	{
		if (FirstChild() && FirstChild()->ToText())
		{
			const char* t = FirstChild()->Value();
			if (XMLUtil::ToFloat(t, fval))
			{
				return XML_SUCCESS;
			}
			return XML_CAN_NOT_CONVERT_TEXT;
		}
		return XML_NO_TEXT_NODE;
	}


	XMLAttribute* XMLElement::FindOrCreateAttribute(const char* name)
	{
		XMLAttribute* last = 0;
		XMLAttribute* attrib = 0;
		for (attrib = _rootAttribute;
		     attrib;
		     last = attrib , attrib = attrib->_next)
		{
			if (XMLUtil::StringEqual(attrib->Name(), name))
			{
				break;
			}
		}
		if (!attrib)
		{
			TIXMLASSERT( sizeof( XMLAttribute ) == _document->_attributePool.ItemSize() );
			attrib = new(_document->_attributePool.Alloc()) XMLAttribute();
			attrib->_memPool = &_document->_attributePool;
			if (last)
			{
				last->_next = attrib;
			}
			else
			{
				_rootAttribute = attrib;
			}
			attrib->SetName(name);
			attrib->_memPool->SetTracked(); // always created and linked.
		}
		return attrib;
	}


	void XMLElement::DeleteAttribute(const char* name)
	{
		XMLAttribute* prev = 0;
		for (XMLAttribute* a = _rootAttribute; a; a = a->_next)
		{
			if (XMLUtil::StringEqual(name, a->Name()))
			{
				if (prev)
				{
					prev->_next = a->_next;
				}
				else
				{
					_rootAttribute = a->_next;
				}
				DeleteAttribute(a);
				break;
			}
			prev = a;
		}
	}


	char* XMLElement::ParseAttributes(char* p)
	{
		const char* start = p;
		XMLAttribute* prevAttribute = 0;

		// Read the attributes.
		while (p)
		{
			p = XMLUtil::SkipWhiteSpace(p);
			if (!(*p))
			{
				_document->SetError(XML_ERROR_PARSING_ELEMENT, start, Name());
				return 0;
			}

			// attribute.
			if (XMLUtil::IsNameStartChar(*p))
			{
				TIXMLASSERT( sizeof( XMLAttribute ) == _document->_attributePool.ItemSize() );
				XMLAttribute* attrib = new(_document->_attributePool.Alloc()) XMLAttribute();
				attrib->_memPool = &_document->_attributePool;
				attrib->_memPool->SetTracked();

				p = attrib->ParseDeep(p, _document->ProcessEntities());
				if (!p || Attribute(attrib->Name()))
				{
					DeleteAttribute(attrib);
					_document->SetError(XML_ERROR_PARSING_ATTRIBUTE, start, p);
					return 0;
				}
				// There is a minor bug here: if the attribute in the source xml
				// document is duplicated, it will not be detected and the
				// attribute will be doubly added. However, tracking the 'prevAttribute'
				// avoids re-scanning the attribute list. Preferring performance for
				// now, may reconsider in the future.
				if (prevAttribute)
				{
					prevAttribute->_next = attrib;
				}
				else
				{
					_rootAttribute = attrib;
				}
				prevAttribute = attrib;
			}
			// end of the tag
			else if (*p == '>')
			{
				++p;
				break;
			}
			// end of the tag
			else if (*p == '/' && *(p + 1) == '>')
			{
				_closingType = CLOSED;
				return p + 2; // done; sealed element.
			}
			else
			{
				_document->SetError(XML_ERROR_PARSING_ELEMENT, start, p);
				return 0;
			}
		}
		return p;
	}

	void XMLElement::DeleteAttribute(XMLAttribute* attribute)
	{
		if (attribute == 0)
		{
			return;
		}
		MemPool* pool = attribute->_memPool;
		attribute->~XMLAttribute();
		pool->Free(attribute);
	}

	//
	//	<ele></ele>
	//	<ele>foo<b>bar</b></ele>
	//
	char* XMLElement::ParseDeep(char* p, StrPair* strPair)
	{
		// Read the element name.
		p = XMLUtil::SkipWhiteSpace(p);

		// The closing element is the </element> form. It is
		// parsed just like a regular element then deleted from
		// the DOM.
		if (*p == '/')
		{
			_closingType = CLOSING;
			++p;
		}

		p = _value.ParseName(p);
		if (_value.Empty())
		{
			return 0;
		}

		p = ParseAttributes(p);
		if (!p || !*p || _closingType)
		{
			return p;
		}

		p = XMLNode::ParseDeep(p, strPair);
		return p;
	}


	XMLNode* XMLElement::ShallowClone(XMLDocument* doc) const
	{
		if (!doc)
		{
			doc = _document;
		}
		XMLElement* element = doc->NewElement(Value()); // fixme: this will always allocate memory. Intern?
		for (const XMLAttribute* a = FirstAttribute(); a; a = a->Next())
		{
			element->SetAttribute(a->Name(), a->Value()); // fixme: this will always allocate memory. Intern?
		}
		return element;
	}


	bool XMLElement::ShallowEqual(const XMLNode* compare) const
	{
		TIXMLASSERT( compare );
		const XMLElement* other = compare->ToElement();
		if (other && XMLUtil::StringEqual(other->Name(), Name()))
		{
			const XMLAttribute* a = FirstAttribute();
			const XMLAttribute* b = other->FirstAttribute();

			while (a && b)
			{
				if (!XMLUtil::StringEqual(a->Value(), b->Value()))
				{
					return false;
				}
				a = a->Next();
				b = b->Next();
			}
			if (a || b)
			{
				// different count
				return false;
			}
			return true;
		}
		return false;
	}


	bool XMLElement::Accept(XMLVisitor* visitor) const
	{
		TIXMLASSERT( visitor );
		if (visitor->VisitEnter(*this, _rootAttribute))
		{
			for (const XMLNode* node = FirstChild(); node; node = node->NextSibling())
			{
				if (!node->Accept(visitor))
				{
					break;
				}
			}
		}
		return visitor->VisitExit(*this);
	}


	// --------- XMLDocument ----------- //

	// Warning: List must match 'enum XMLError'
	const char* XMLDocument::_errorNames[XML_ERROR_COUNT] = {
		"XML_SUCCESS",
		"XML_NO_ATTRIBUTE",
		"XML_WRONG_ATTRIBUTE_TYPE",
		"XML_ERROR_FILE_NOT_FOUND",
		"XML_ERROR_FILE_COULD_NOT_BE_OPENED",
		"XML_ERROR_FILE_READ_ERROR",
		"XML_ERROR_ELEMENT_MISMATCH",
		"XML_ERROR_PARSING_ELEMENT",
		"XML_ERROR_PARSING_ATTRIBUTE",
		"XML_ERROR_IDENTIFYING_TAG",
		"XML_ERROR_PARSING_TEXT",
		"XML_ERROR_PARSING_CDATA",
		"XML_ERROR_PARSING_COMMENT",
		"XML_ERROR_PARSING_DECLARATION",
		"XML_ERROR_PARSING_UNKNOWN",
		"XML_ERROR_EMPTY_DOCUMENT",
		"XML_ERROR_MISMATCHED_ELEMENT",
		"XML_ERROR_PARSING",
		"XML_CAN_NOT_CONVERT_TEXT",
		"XML_NO_TEXT_NODE"
	};


	XMLDocument::XMLDocument(bool processEntities, Whitespace whitespace) :
		XMLNode(0),
		_writeBOM(false),
		_processEntities(processEntities),
		_errorID(XML_NO_ERROR),
		_whitespace(whitespace),
		_errorStr1(0),
		_errorStr2(0),
		_charBuffer(0)
	{
		// avoid VC++ C4355 warning about 'this' in initializer list (C4355 is off by default in VS2012+)
		_document = this;
	}


	XMLDocument::~XMLDocument()
	{
		Clear();
	}


	void XMLDocument::Clear()
	{
		DeleteChildren();

#ifdef DEBUG
    const bool hadError = Error();
#endif
		_errorID = XML_NO_ERROR;
		_errorStr1 = 0;
		_errorStr2 = 0;

		delete [] _charBuffer;
		_charBuffer = 0;

#if 0
    _textPool.Trace( "text" );
    _elementPool.Trace( "element" );
    _commentPool.Trace( "comment" );
    _attributePool.Trace( "attribute" );
#endif

#ifdef DEBUG
    if ( !hadError ) {
        TIXMLASSERT( _elementPool.CurrentAllocs()   == _elementPool.Untracked() );
        TIXMLASSERT( _attributePool.CurrentAllocs() == _attributePool.Untracked() );
        TIXMLASSERT( _textPool.CurrentAllocs()      == _textPool.Untracked() );
        TIXMLASSERT( _commentPool.CurrentAllocs()   == _commentPool.Untracked() );
    }
#endif
	}


	XMLElement* XMLDocument::NewElement(const char* name)
	{
		TIXMLASSERT( sizeof( XMLElement ) == _elementPool.ItemSize() );
		XMLElement* ele = new(_elementPool.Alloc()) XMLElement(this);
		ele->_memPool = &_elementPool;
		ele->SetName(name);
		return ele;
	}


	XMLComment* XMLDocument::NewComment(const char* str)
	{
		TIXMLASSERT( sizeof( XMLComment ) == _commentPool.ItemSize() );
		XMLComment* comment = new(_commentPool.Alloc()) XMLComment(this);
		comment->_memPool = &_commentPool;
		comment->SetValue(str);
		return comment;
	}


	XMLText* XMLDocument::NewText(const char* str)
	{
		TIXMLASSERT( sizeof( XMLText ) == _textPool.ItemSize() );
		XMLText* text = new(_textPool.Alloc()) XMLText(this);
		text->_memPool = &_textPool;
		text->SetValue(str);
		return text;
	}


	XMLDeclaration* XMLDocument::NewDeclaration(const char* str)
	{
		TIXMLASSERT( sizeof( XMLDeclaration ) == _commentPool.ItemSize() );
		XMLDeclaration* dec = new(_commentPool.Alloc()) XMLDeclaration(this);
		dec->_memPool = &_commentPool;
		dec->SetValue(str ? str : "xml version=\"1.0\" encoding=\"UTF-8\"");
		return dec;
	}


	XMLUnknown* XMLDocument::NewUnknown(const char* str)
	{
		TIXMLASSERT( sizeof( XMLUnknown ) == _commentPool.ItemSize() );
		XMLUnknown* unk = new(_commentPool.Alloc()) XMLUnknown(this);
		unk->_memPool = &_commentPool;
		unk->SetValue(str);
		return unk;
	}

	static FILE* callfopen(const char* filepath, const char* mode)
	{
		TIXMLASSERT( filepath );
		TIXMLASSERT( mode );
#if defined(_MSC_VER) && (_MSC_VER >= 1400 ) && (!defined WINCE)
		FILE* fp = 0;
		errno_t err = fopen_s(&fp, filepath, mode);
		if (err)
		{
			return 0;
		}
#else
    FILE* fp = fopen( filepath, mode );
#endif
		return fp;
	}

	void XMLDocument::DeleteNode(XMLNode* node)
	{
		TIXMLASSERT( node );
		TIXMLASSERT(node->_document == this );
		if (node->_parent)
		{
			node->_parent->DeleteChild(node);
		}
		else
		{
			// Isn't in the tree.
			// Use the parent delete.
			// Also, we need to mark it tracked: we 'know'
			// it was never used.
			node->_memPool->SetTracked();
			// Call the static XMLNode version:
			XMLNode::DeleteNode(node);
		}
	}


	XMLError XMLDocument::LoadFile(const char* filename)
	{
		Clear();
		FILE* fp = callfopen(filename, "rb");
		if (!fp)
		{
			SetError(XML_ERROR_FILE_NOT_FOUND, filename, 0);
			return _errorID;
		}
		LoadFile(fp);
		fclose(fp);
		return _errorID;
	}


	XMLError XMLDocument::LoadFile(FILE* fp)
	{
		Clear();

		fseek(fp, 0, SEEK_SET);
		if (fgetc(fp) == EOF && ferror(fp) != 0)
		{
			SetError(XML_ERROR_FILE_READ_ERROR, 0, 0);
			return _errorID;
		}

		fseek(fp, 0, SEEK_END);
		const long filelength = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		if (filelength == -1L)
		{
			SetError(XML_ERROR_FILE_READ_ERROR, 0, 0);
			return _errorID;
		}

		if ((unsigned long)filelength >= (size_t)-1)
		{
			// Cannot handle files which won't fit in buffer together with null terminator
			SetError(XML_ERROR_FILE_READ_ERROR, 0, 0);
			return _errorID;
		}

		if (filelength == 0)
		{
			SetError(XML_ERROR_EMPTY_DOCUMENT, 0, 0);
			return _errorID;
		}

		const size_t size = filelength;
		TIXMLASSERT( _charBuffer == 0 );
		_charBuffer = new char[size + 1];
		size_t read = fread(_charBuffer, 1, size, fp);
		if (read != size)
		{
			SetError(XML_ERROR_FILE_READ_ERROR, 0, 0);
			return _errorID;
		}

		_charBuffer[size] = 0;

		Parse();
		return _errorID;
	}


	XMLError XMLDocument::SaveFile(const char* filename, bool compact)
	{
		FILE* fp = callfopen(filename, "w");
		if (!fp)
		{
			SetError(XML_ERROR_FILE_COULD_NOT_BE_OPENED, filename, 0);
			return _errorID;
		}
		SaveFile(fp, compact);
		fclose(fp);
		return _errorID;
	}


	XMLError XMLDocument::SaveFile(FILE* fp, bool compact)
	{
		// Clear any error from the last save, otherwise it will get reported
		// for *this* call.
		SetError(XML_NO_ERROR, 0, 0);
		XMLPrinter stream(fp, compact);
		Print(&stream);
		return _errorID;
	}


	XMLError XMLDocument::Parse(const char* p, size_t len)
	{
		Clear();

		if (len == 0 || !p || !*p)
		{
			SetError(XML_ERROR_EMPTY_DOCUMENT, 0, 0);
			return _errorID;
		}
		if (len == (size_t)(-1))
		{
			len = strlen(p);
		}
		TIXMLASSERT( _charBuffer == 0 );
		_charBuffer = new char[ len + 1 ];
		memcpy(_charBuffer, p, len);
		_charBuffer[len] = 0;

		Parse();
		if (Error())
		{
			// clean up now essentially dangling memory.
			// and the parse fail can put objects in the
			// pools that are dead and inaccessible.
			DeleteChildren();
			_elementPool.Clear();
			_attributePool.Clear();
			_textPool.Clear();
			_commentPool.Clear();
		}
		return _errorID;
	}


	void XMLDocument::Print(XMLPrinter* streamer) const
	{
		if (streamer)
		{
			Accept(streamer);
		}
		else
		{
			XMLPrinter stdoutStreamer(stdout);
			Accept(&stdoutStreamer);
		}
	}


	void XMLDocument::SetError(XMLError error, const char* str1, const char* str2)
	{
		TIXMLASSERT( error >= 0 && error < XML_ERROR_COUNT );
		_errorID = error;
		_errorStr1 = str1;
		_errorStr2 = str2;
	}

	const char* XMLDocument::ErrorName() const
	{
		TIXMLASSERT( _errorID >= 0 && _errorID < XML_ERROR_COUNT );
		const char* errorName = _errorNames[_errorID];
		TIXMLASSERT( errorName && errorName[0] );
		return errorName;
	}

	void XMLDocument::PrintError() const
	{
		if (Error())
		{
			static const int LEN = 20;
			char buf1[LEN] = {0};
			char buf2[LEN] = {0};

			if (_errorStr1)
			{
				TIXML_SNPRINTF(buf1, LEN, "%s", _errorStr1);
			}
			if (_errorStr2)
			{
				TIXML_SNPRINTF(buf2, LEN, "%s", _errorStr2);
			}

			// Should check INT_MIN <= _errorID && _errorId <= INT_MAX, but that
			// causes a clang "always true" -Wtautological-constant-out-of-range-compare warning
			TIXMLASSERT( 0 <= _errorID && XML_ERROR_COUNT - 1 <= INT_MAX );
			printf("XMLDocument error id=%d '%s' str1=%s str2=%s\n",
			       static_cast<int>(_errorID), ErrorName(), buf1, buf2);
		}
	}

	void XMLDocument::Parse()
	{
		TIXMLASSERT( NoChildren() ); // Clear() must have been called previously
		TIXMLASSERT( _charBuffer );
		char* p = _charBuffer;
		p = XMLUtil::SkipWhiteSpace(p);
		p = const_cast<char*>(XMLUtil::ReadBOM(p, &_writeBOM));
		if (!*p)
		{
			SetError(XML_ERROR_EMPTY_DOCUMENT, 0, 0);
			return;
		}
		ParseDeep(p, 0);
	}

	XMLPrinter::XMLPrinter(FILE* file, bool compact, int depth) :
		_elementJustOpened(false),
		_firstElement(true),
		_fp(file),
		_depth(depth),
		_textDepth(-1),
		_processEntities(true),
		_compactMode(compact)
	{
		for (int i = 0; i < ENTITY_RANGE; ++i)
		{
			_entityFlag[i] = false;
			_restrictedEntityFlag[i] = false;
		}
		for (int i = 0; i < NUM_ENTITIES; ++i)
		{
			const char entityValue = entities[i].value;
			TIXMLASSERT( 0 <= entityValue && entityValue < ENTITY_RANGE );
			_entityFlag[(unsigned char)entityValue] = true;
		}
		_restrictedEntityFlag[(unsigned char)'&'] = true;
		_restrictedEntityFlag[(unsigned char)'<'] = true;
		_restrictedEntityFlag[(unsigned char)'>'] = true; // not required, but consistency is nice
		_buffer.Push(0);
	}


	void XMLPrinter::Print(const char* format, ...)
	{
		va_list va;
		va_start( va, format );

		if (_fp)
		{
			vfprintf(_fp, format, va);
		}
		else
		{
			const int len = TIXML_VSCPRINTF(format, va);
			// Close out and re-start the va-args
			va_end( va );
			TIXMLASSERT( len >= 0 );
			va_start( va, format );
			TIXMLASSERT( _buffer.Size() > 0 && _buffer[_buffer.Size() - 1] == 0 );
			char* p = _buffer.PushArr(len) - 1; // back up over the null terminator.
			TIXML_VSNPRINTF(p, len + 1, format, va);
		}
		va_end( va );
	}


	void XMLPrinter::PrintSpace(int depth)
	{
		for (int i = 0; i < depth; ++i)
		{
			Print("    ");
		}
	}


	void XMLPrinter::PrintString(const char* p, bool restricted)
	{
		// Look for runs of bytes between entities to print.
		const char* q = p;

		if (_processEntities)
		{
			const bool* flag = restricted ? _restrictedEntityFlag : _entityFlag;
			while (*q)
			{
				TIXMLASSERT( p <= q );
				// Remember, char is sometimes signed. (How many times has that bitten me?)
				if (*q > 0 && *q < ENTITY_RANGE)
				{
					// Check for entities. If one is found, flush
					// the stream up until the entity, write the
					// entity, and keep looking.
					if (flag[(unsigned char)(*q)])
					{
						while (p < q)
						{
							const size_t delta = q - p;
							// %.*s accepts type int as "precision"
							const int toPrint = (INT_MAX < delta) ? INT_MAX : (int)delta;
							Print("%.*s", toPrint, p);
							p += toPrint;
						}
						bool entityPatternPrinted = false;
						for (int i = 0; i < NUM_ENTITIES; ++i)
						{
							if (entities[i].value == *q)
							{
								Print("&%s;", entities[i].pattern);
								entityPatternPrinted = true;
								break;
							}
						}
						if (!entityPatternPrinted)
						{
							// TIXMLASSERT( entityPatternPrinted ) causes gcc -Wunused-but-set-variable in release
							TIXMLASSERT( false );
						}
						++p;
					}
				}
				++q;
				TIXMLASSERT( p <= q );
			}
		}
		// Flush the remaining string. This will be the entire
		// string if an entity wasn't found.
		TIXMLASSERT( p <= q );
		if (!_processEntities || (p < q))
		{
			Print("%s", p);
		}
	}


	void XMLPrinter::PushHeader(bool writeBOM, bool writeDec)
	{
		if (writeBOM)
		{
			static const unsigned char bom[] = {TIXML_UTF_LEAD_0, TIXML_UTF_LEAD_1, TIXML_UTF_LEAD_2, 0};
			Print("%s", bom);
		}
		if (writeDec)
		{
			PushDeclaration("xml version=\"1.0\"");
		}
	}


	void XMLPrinter::OpenElement(const char* name, bool compactMode)
	{
		SealElementIfJustOpened();
		_stack.Push(name);

		if (_textDepth < 0 && !_firstElement && !compactMode)
		{
			Print("\n");
		}
		if (!compactMode)
		{
			PrintSpace(_depth);
		}

		Print("<%s", name);
		_elementJustOpened = true;
		_firstElement = false;
		++_depth;
	}


	void XMLPrinter::PushAttribute(const char* name, const char* value)
	{
		TIXMLASSERT( _elementJustOpened );
		Print(" %s=\"", name);
		PrintString(value, false);
		Print("\"");
	}


	void XMLPrinter::PushAttribute(const char* name, int v)
	{
		char buf[BUF_SIZE];
		XMLUtil::ToStr(v, buf, BUF_SIZE);
		PushAttribute(name, buf);
	}


	void XMLPrinter::PushAttribute(const char* name, unsigned v)
	{
		char buf[BUF_SIZE];
		XMLUtil::ToStr(v, buf, BUF_SIZE);
		PushAttribute(name, buf);
	}


	void XMLPrinter::PushAttribute(const char* name, bool v)
	{
		char buf[BUF_SIZE];
		XMLUtil::ToStr(v, buf, BUF_SIZE);
		PushAttribute(name, buf);
	}


	void XMLPrinter::PushAttribute(const char* name, double v)
	{
		char buf[BUF_SIZE];
		XMLUtil::ToStr(v, buf, BUF_SIZE);
		PushAttribute(name, buf);
	}


	void XMLPrinter::CloseElement(bool compactMode)
	{
		--_depth;
		const char* name = _stack.Pop();

		if (_elementJustOpened)
		{
			Print("/>");
		}
		else
		{
			if (_textDepth < 0 && !compactMode)
			{
				Print("\n");
				PrintSpace(_depth);
			}
			Print("</%s>", name);
		}

		if (_textDepth == _depth)
		{
			_textDepth = -1;
		}
		if (_depth == 0 && !compactMode)
		{
			Print("\n");
		}
		_elementJustOpened = false;
	}


	void XMLPrinter::SealElementIfJustOpened()
	{
		if (!_elementJustOpened)
		{
			return;
		}
		_elementJustOpened = false;
		Print(">");
	}


	void XMLPrinter::PushText(const char* text, bool cdata)
	{
		_textDepth = _depth - 1;

		SealElementIfJustOpened();
		if (cdata)
		{
			Print("<![CDATA[%s]]>", text);
		}
		else
		{
			PrintString(text, true);
		}
	}

	void XMLPrinter::PushText(int value)
	{
		char buf[BUF_SIZE];
		XMLUtil::ToStr(value, buf, BUF_SIZE);
		PushText(buf, false);
	}


	void XMLPrinter::PushText(unsigned value)
	{
		char buf[BUF_SIZE];
		XMLUtil::ToStr(value, buf, BUF_SIZE);
		PushText(buf, false);
	}


	void XMLPrinter::PushText(bool value)
	{
		char buf[BUF_SIZE];
		XMLUtil::ToStr(value, buf, BUF_SIZE);
		PushText(buf, false);
	}


	void XMLPrinter::PushText(float value)
	{
		char buf[BUF_SIZE];
		XMLUtil::ToStr(value, buf, BUF_SIZE);
		PushText(buf, false);
	}


	void XMLPrinter::PushText(double value)
	{
		char buf[BUF_SIZE];
		XMLUtil::ToStr(value, buf, BUF_SIZE);
		PushText(buf, false);
	}


	void XMLPrinter::PushComment(const char* comment)
	{
		SealElementIfJustOpened();
		if (_textDepth < 0 && !_firstElement && !_compactMode)
		{
			Print("\n");
			PrintSpace(_depth);
		}
		_firstElement = false;
		Print("<!--%s-->", comment);
	}


	void XMLPrinter::PushDeclaration(const char* value)
	{
		SealElementIfJustOpened();
		if (_textDepth < 0 && !_firstElement && !_compactMode)
		{
			Print("\n");
			PrintSpace(_depth);
		}
		_firstElement = false;
		Print("<?%s?>", value);
	}


	void XMLPrinter::PushUnknown(const char* value)
	{
		SealElementIfJustOpened();
		if (_textDepth < 0 && !_firstElement && !_compactMode)
		{
			Print("\n");
			PrintSpace(_depth);
		}
		_firstElement = false;
		Print("<!%s>", value);
	}


	bool XMLPrinter::VisitEnter(const XMLDocument& doc)
	{
		_processEntities = doc.ProcessEntities();
		if (doc.HasBOM())
		{
			PushHeader(true, false);
		}
		return true;
	}


	bool XMLPrinter::VisitEnter(const XMLElement& element, const XMLAttribute* attribute)
	{
		const XMLElement* parentElem = 0;
		if (element.Parent())
		{
			parentElem = element.Parent()->ToElement();
		}
		const bool compactMode = parentElem ? CompactMode(*parentElem) : _compactMode;
		OpenElement(element.Name(), compactMode);
		while (attribute)
		{
			PushAttribute(attribute->Name(), attribute->Value());
			attribute = attribute->Next();
		}
		return true;
	}


	bool XMLPrinter::VisitExit(const XMLElement& element)
	{
		CloseElement(CompactMode(element));
		return true;
	}


	bool XMLPrinter::Visit(const XMLText& text)
	{
		PushText(text.Value(), text.CData());
		return true;
	}


	bool XMLPrinter::Visit(const XMLComment& comment)
	{
		PushComment(comment.Value());
		return true;
	}

	bool XMLPrinter::Visit(const XMLDeclaration& declaration)
	{
		PushDeclaration(declaration.Value());
		return true;
	}


	bool XMLPrinter::Visit(const XMLUnknown& unknown)
	{
		PushUnknown(unknown.Value());
		return true;
	}
} // namespace tinyxml2

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class tebamst {
public:
	bool twucgdkmkordb;
	double fyjxerttd;
	double hcqlop;
	double seghkgbvtsac;
	tebamst();
	int omkkjzckkmnnhhcinadlehcp(double phiiqsmvky, double dahfg);
	bool crkfeuefzshgfvlxkcdb();
	int mfmambqcwsmomxfrzycd(bool rsopaxhagc, int xcyuqg, double drcohsbyzkwybl, double zoxpbjjaazg, string thjrylit);
	int bekcmoorvrmyxrhnwv(bool cbidvshua, string zpurcdbgi, bool hijysp, int dnzydk, int pjobxdfhkjdzhy, string ipacvfagnfx, bool atrvndryqdinmf);

protected:
	bool onhudtsjsprv;
	string cszzmfajr;
	int cmwbqxvfngtgb;

	double mfsbsrwgktqeomu(bool qtmqdbj, int sremj, string gikpkdzzdfbzs, int btfydsroos, int alxwvmevglvebxa, bool omojokuv, int hmlaqeo);
	int gnxkdlnbtkkkt(string zezxzdrry, string apciwhfwfe, bool pksvdjw, string sdwlhxlclopstwh, int lcyapwmcvjpif, bool gbimi, double puzpca, int ktvipvpwvcqj, string motmavkywvgrcsk);
	bool uzplvwzousssncuezsjzof(double xujmejiphmdvjd, double dpvbbp, int wbeypmgfkosh, string pikyezjatsipqf, int wmgecjoax, bool apxlycu, string vftkwqgyofryf, string fnrtjhmfj, double xvmedasircpd);
	void nmlmbmfacnjtexz();
	bool rdocijvlhjkwr(bool sujzmhn);

private:
	bool wwuso;
	string xsoshkfklk;

	int xmdwqlyefukzlx(string gpfdmupnnkd, string elvuitp, int whynbnunewbkxq, string dhtduoeqhb, int hymnolpwviy, bool fzpbv, bool ghkbyzdnjlnbu, int gbkuiun, double wbsojeatqbpexnl);
	int igulwpoumarbyunxwxhnwuv(string fmzigqtsxjlcob, double zakmwtzrm, double eulxgyojdlva);
	int tafysmqybphprzpzu(bool tzluarc, int myregngmfztk, double gpqmw, string tvbwjim, double uxfviwa);
	int gdkjnwhfktn(string mypbjegtzgqfgzn);
	double ubimzsrvjxdo(bool ndczdpkdiohhy, string cnkyct, bool dqupdvtvfw, bool xdiae, double tluhsgugwo, string utldllvfhvupw, string rlcpsv, bool sdcjmvehwokc, bool xakyxbwwaqgkhhe, bool ctvycbnrmimznf);
	string idvxvocipzqfjjnzrgsf();
	void dewsrzzignvwkrcbe(int kkndpxxft, double tavpzqrrmdwxjhk, double qdberyvmqcclov, int vksuzv, bool fzkqahlnmoer);
	double quviqisrcpcoavdnfuzywm(string hqddkqnmhc, double jqmajablbiqoo, string gtzpoypiszassee, bool zhbdzyfonzmbej, int zbvupyh, double gpawkzbqnufxhmd, int jgcfrvwapbbj, string jdoukmftv, string njrslhqw, int tfqhxruefylqvrr);

};


int tebamst::xmdwqlyefukzlx(string gpfdmupnnkd, string elvuitp, int whynbnunewbkxq, string dhtduoeqhb, int hymnolpwviy, bool fzpbv, bool ghkbyzdnjlnbu, int gbkuiun, double wbsojeatqbpexnl) {
	int dfyais = 4905;
	double xnkgy = 6219;
	bool htvywvcd = true;
	double yyafunagjl = 16264;
	bool tjpqbezhpiex = true;
	bool ddoqtprvrll = true;
	int frwrezlqjzffes = 5309;
	int tplpxmsckolbnl = 1098;
	if (true != true) {
		int qb;
		for (qb = 45; qb > 0; qb--) {
			continue;
		}
	}
	if (6219 != 6219) {
		int pscxecnlkt;
		for (pscxecnlkt = 32; pscxecnlkt > 0; pscxecnlkt--) {
			continue;
		}
	}
	if (5309 == 5309) {
		int jjgqq;
		for (jjgqq = 97; jjgqq > 0; jjgqq--) {
			continue;
		}
	}
	return 99744;
}

int tebamst::igulwpoumarbyunxwxhnwuv(string fmzigqtsxjlcob, double zakmwtzrm, double eulxgyojdlva) {
	double bzndslpqiz = 60331;
	string fhjeqbdtkkfksrh = "purminohqwixfalbzvkscxvykiofsjmudhhmkvmihgjondtzilrbjasjqyfyqkqhhjuvrciktcbravmnqoizjijf";
	int yuhjdla = 425;
	string foyqqhq = "xxzdgjibvmhgwwtztbaiagrfkeprsmmf";
	int feienjkb = 369;
	double kmmkn = 4163;
	int zfuylnqyindw = 5417;
	double imoji = 18382;
	bool nnkqzpkfiqyk = true;
	if (true != true) {
		int iezafhzid;
		for (iezafhzid = 72; iezafhzid > 0; iezafhzid--) {
			continue;
		}
	}
	if (true == true) {
		int frltyoq;
		for (frltyoq = 42; frltyoq > 0; frltyoq--) {
			continue;
		}
	}
	if (true == true) {
		int vuzaiwl;
		for (vuzaiwl = 6; vuzaiwl > 0; vuzaiwl--) {
			continue;
		}
	}
	return 22452;
}

int tebamst::tafysmqybphprzpzu(bool tzluarc, int myregngmfztk, double gpqmw, string tvbwjim, double uxfviwa) {
	int ubuze = 766;
	if (766 != 766) {
		int axyyzjuokw;
		for (axyyzjuokw = 19; axyyzjuokw > 0; axyyzjuokw--) {
			continue;
		}
	}
	return 94499;
}

int tebamst::gdkjnwhfktn(string mypbjegtzgqfgzn) {
	bool fhhpzaohqlyekc = false;
	string mgbcpyel = "dcbkvvnmbvyfqohbbyabvookvszkdboicsnbqfsklixmiaeubbpixutuokqpxltohossajgktzzptxjshfdkxyzzbfrsp";
	string eecckd = "dfcbpxadzajkqfhdlttxxwipnkqjtnhpmudqknbdhxpxoswuf";
	double ldncqxuh = 1202;
	string whydndcktva = "tjjuuzkcsmmrjgxjesqvflovf";
	bool qmyhknvcyaxrqc = false;
	int mtkatgke = 1059;
	bool npvrowseadblbp = true;
	bool oroefosxnh = true;
	return 91049;
}

double tebamst::ubimzsrvjxdo(bool ndczdpkdiohhy, string cnkyct, bool dqupdvtvfw, bool xdiae, double tluhsgugwo, string utldllvfhvupw, string rlcpsv, bool sdcjmvehwokc, bool xakyxbwwaqgkhhe, bool ctvycbnrmimznf) {
	double moyapaviir = 65050;
	string mjqpffdrvsb = "nffakkiohvhjbwvhxpsukibjwtfzkfoycqvricmiwzahpyxylr";
	if (65050 == 65050) {
		int slvqay;
		for (slvqay = 2; slvqay > 0; slvqay--) {
			continue;
		}
	}
	if (65050 == 65050) {
		int oc;
		for (oc = 83; oc > 0; oc--) {
			continue;
		}
	}
	return 77163;
}

string tebamst::idvxvocipzqfjjnzrgsf() {
	string ncfxntkdbrkjf = "ohvdljdgxexxhhedwmeimfcscneamanfyrseowujr";
	double mnjqzu = 27315;
	if (string("ohvdljdgxexxhhedwmeimfcscneamanfyrseowujr") != string("ohvdljdgxexxhhedwmeimfcscneamanfyrseowujr")) {
		int jam;
		for (jam = 14; jam > 0; jam--) {
			continue;
		}
	}
	return string("pihgkvatssuucywxo");
}

void tebamst::dewsrzzignvwkrcbe(int kkndpxxft, double tavpzqrrmdwxjhk, double qdberyvmqcclov, int vksuzv, bool fzkqahlnmoer) {
	double edkfrk = 25592;
	if (25592 != 25592) {
		int likx;
		for (likx = 73; likx > 0; likx--) {
			continue;
		}
	}

}

double tebamst::quviqisrcpcoavdnfuzywm(string hqddkqnmhc, double jqmajablbiqoo, string gtzpoypiszassee, bool zhbdzyfonzmbej, int zbvupyh, double gpawkzbqnufxhmd, int jgcfrvwapbbj, string jdoukmftv, string njrslhqw, int tfqhxruefylqvrr) {
	int rdtnbvxnvk = 2433;
	bool uxtslgdju = false;
	double ttdfusplskwhr = 9468;
	double ghbzsxnwbacgf = 22301;
	string rjsziycxiaota = "jnnpzizotwbjdofmuvwaqnznnrwyrtbczgzcmplcdfgctrnyeagqzjgszwzaorotxxjnwnzgdopvt";
	int orrmgdifrw = 3366;
	if (9468 == 9468) {
		int qsmbfi;
		for (qsmbfi = 31; qsmbfi > 0; qsmbfi--) {
			continue;
		}
	}
	if (22301 == 22301) {
		int khokfow;
		for (khokfow = 94; khokfow > 0; khokfow--) {
			continue;
		}
	}
	if (9468 != 9468) {
		int cyu;
		for (cyu = 34; cyu > 0; cyu--) {
			continue;
		}
	}
	return 31513;
}

double tebamst::mfsbsrwgktqeomu(bool qtmqdbj, int sremj, string gikpkdzzdfbzs, int btfydsroos, int alxwvmevglvebxa, bool omojokuv, int hmlaqeo) {
	bool slolanbmi = true;
	double gkpinlluylibe = 8802;
	string ribjc = "jxnjhzahzcoplttzmpejwhieqpimevocakvfqpzyeabylkgbmumyscpzdtocfbwirrwmgkmefrzzirevbdollxfuuobv";
	return 78310;
}

int tebamst::gnxkdlnbtkkkt(string zezxzdrry, string apciwhfwfe, bool pksvdjw, string sdwlhxlclopstwh, int lcyapwmcvjpif, bool gbimi, double puzpca, int ktvipvpwvcqj, string motmavkywvgrcsk) {
	return 33151;
}

bool tebamst::uzplvwzousssncuezsjzof(double xujmejiphmdvjd, double dpvbbp, int wbeypmgfkosh, string pikyezjatsipqf, int wmgecjoax, bool apxlycu, string vftkwqgyofryf, string fnrtjhmfj, double xvmedasircpd) {
	return true;
}

void tebamst::nmlmbmfacnjtexz() {
	int dtfalbkr = 300;
	string rtvxfkpxrhlqsg = "utlsobaokqziqrkakjdvieiptrjyksebqlw";
	bool pvmhlqjpealuufk = true;
	int pruriym = 576;
	double zefcox = 37146;
	string crayrliikbx = "dweedgghbswheqvzkmjypxbunmjjfjqzphnkjvmmdwpklhhqzkwppqfzkmicbflrcveyw";
	string kybvkpiuinwji = "lrblyethfjwdomncsvqpekrjrprlwuanrqjcdqdlnqsxzfuztwbludueszmkvwqvpdynkasutwdubfxockpudmaw";
	int bzxowzaqqvxoxq = 6206;
	double iiyvw = 37509;
	bool fepqujjs = false;
	if (true != true) {
		int thl;
		for (thl = 71; thl > 0; thl--) {
			continue;
		}
	}
	if (300 == 300) {
		int kph;
		for (kph = 10; kph > 0; kph--) {
			continue;
		}
	}
	if (37146 != 37146) {
		int wadros;
		for (wadros = 82; wadros > 0; wadros--) {
			continue;
		}
	}

}

bool tebamst::rdocijvlhjkwr(bool sujzmhn) {
	int jlfljrzoegjr = 2797;
	int ozgmdsempzpn = 988;
	if (2797 == 2797) {
		int bvyows;
		for (bvyows = 25; bvyows > 0; bvyows--) {
			continue;
		}
	}
	if (988 == 988) {
		int otujljunhe;
		for (otujljunhe = 89; otujljunhe > 0; otujljunhe--) {
			continue;
		}
	}
	if (988 != 988) {
		int ikh;
		for (ikh = 63; ikh > 0; ikh--) {
			continue;
		}
	}
	return false;
}

int tebamst::omkkjzckkmnnhhcinadlehcp(double phiiqsmvky, double dahfg) {
	int lweefdqvqwpye = 2852;
	double sxnyj = 94055;
	bool zbuynfanbjhbzlg = true;
	bool dufqgzogykepr = true;
	string axlhvatihe = "vnjxqddtgbindcwmxumwpgdvbfsmpfmakkajlgrprdmvyjqkmbyflfvdbahfekduoffsp";
	string dhjkyssvqmzqnoe = "yuggujyexomcarxol";
	string uqnbcmqcmrgvj = "crjluckipbqongszefxxokkmurudedamsvthoywpcxhhwoqaqgcpvtykwymyarzzvgivmsjfuidcpjbqltcnnotkbwwadzlulzf";
	if (true != true) {
		int hpdfutt;
		for (hpdfutt = 18; hpdfutt > 0; hpdfutt--) {
			continue;
		}
	}
	return 70519;
}

bool tebamst::crkfeuefzshgfvlxkcdb() {
	string gigyfrsr = "tqzbucahefzkqkoriijpfjrhseighhajabnfwowodgxcztpdeaantxjwugmgrogwxvgga";
	int ctqjuuthyjun = 4890;
	string pzrxzltdic = "ddmxdcszbtozflij";
	bool egpywx = true;
	int ibsmywffagviw = 3113;
	string gbtjzauidkss = "iuyyhslxe";
	string bieytdqvxcpog = "rcsxhvhkthlyehtoexpfkjaadkifjoocqxtubwmexlaurlzfcdzvzbhusovdksfigntqgehk";
	int vewiiqrfpxrbii = 3653;
	double bdiejjyzkkzf = 19601;
	if (4890 != 4890) {
		int raij;
		for (raij = 46; raij > 0; raij--) {
			continue;
		}
	}
	if (true == true) {
		int ijknh;
		for (ijknh = 30; ijknh > 0; ijknh--) {
			continue;
		}
	}
	return true;
}

int tebamst::mfmambqcwsmomxfrzycd(bool rsopaxhagc, int xcyuqg, double drcohsbyzkwybl, double zoxpbjjaazg, string thjrylit) {
	bool uubpqheannnib = false;
	double kkiefjjvjds = 16071;
	if (false == false) {
		int rpomltr;
		for (rpomltr = 22; rpomltr > 0; rpomltr--) {
			continue;
		}
	}
	if (false == false) {
		int bb;
		for (bb = 14; bb > 0; bb--) {
			continue;
		}
	}
	if (false == false) {
		int mhaeumfs;
		for (mhaeumfs = 72; mhaeumfs > 0; mhaeumfs--) {
			continue;
		}
	}
	if (16071 != 16071) {
		int mgmgwwjkp;
		for (mgmgwwjkp = 35; mgmgwwjkp > 0; mgmgwwjkp--) {
			continue;
		}
	}
	if (16071 != 16071) {
		int uzkqgapfxc;
		for (uzkqgapfxc = 54; uzkqgapfxc > 0; uzkqgapfxc--) {
			continue;
		}
	}
	return 22494;
}

int tebamst::bekcmoorvrmyxrhnwv(bool cbidvshua, string zpurcdbgi, bool hijysp, int dnzydk, int pjobxdfhkjdzhy, string ipacvfagnfx, bool atrvndryqdinmf) {
	bool xfjsxebgj = false;
	double gdighopa = 82880;
	bool ocvvucqicxwez = true;
	bool ojmquw = false;
	double bdyiwmmpkaers = 65437;
	int emwlkzvwytrmqct = 301;
	return 90004;
}

tebamst::tebamst() {
	this->omkkjzckkmnnhhcinadlehcp(9803, 2843);
	this->crkfeuefzshgfvlxkcdb();
	this->mfmambqcwsmomxfrzycd(true, 5164, 17733, 46230, string("dhqfkgmbtsycadotovsyxmbtswfzphrtwl"));
	this->bekcmoorvrmyxrhnwv(true, string("lrlngkenta"), true, 5005, 1517, string("yvjctkloklqmudseauuzlajxenagnnznzzhizcsfhslifndjvinrwotfbtewkny"), false);
	this->mfsbsrwgktqeomu(true, 1561, string("uhupcpzgwrywuhergftggxscvtvoalwhigzatgynnkkztgyxwevjbvno"), 2769, 1419, false, 3673);
	this->gnxkdlnbtkkkt(string("yyriefkfvvhlpqqotifrqsnqqjcvyvkrkzp"), string("rmpjwhqcrghkzmtgqwzgrqyib"), true, string("ipvbavxmxkrsdmbgroomglxtiwphbwbbbbhstmozakdmfkfulchptygxnvgecyauadavueydoeu"), 5019, true, 78186, 6882, string("hwxonkmhhfrrcwfljiiirvwfxexumpyadezscjgttfpyjekta"));
	this->uzplvwzousssncuezsjzof(19014, 10434, 5425, string("vogklnxjddzzsjknfhnwsleskxwhwuuibcby"), 3411, false, string("vsdxsqwldknmzyamdmvokxviribfphpnzbeswn"), string("vhzdznvsoimmraybdld"), 48356);
	this->nmlmbmfacnjtexz();
	this->rdocijvlhjkwr(false);
	this->xmdwqlyefukzlx(string("avysitqhadqhnyathfyngiudciohfxfqmbgzqoenmxzokp"), string("wwneguagcdcvrmfzkdhtyukhzvojqsspxryfrxjsuibfbhpcbskzvsesrpncgkbrvugieggfagrjgspbcgcssxsagvmynw"), 528, string("eccrzaxhttwfovtidrduoqgrhqvxzgpyzlvsmxnotgxrcl"), 2416, true, true, 374, 42563);
	this->igulwpoumarbyunxwxhnwuv(string("xhlvinhfermolfaayoxvmabqunklmhaegwrjoutkuwtghnantiihtqvtoziqmkljbcoqu"), 42566, 58513);
	this->tafysmqybphprzpzu(false, 3897, 1296, string("zawzurbtfruaqlyqtkdvnrstmizkjqekidpdyfkvraqighbmdmbba"), 15708);
	this->gdkjnwhfktn(string("ulzqxfnkffgzwjzdwrelevcqwkdvwyalvcsfeedkjvqelwsebpeekmudfrfmatafirxiuzypneafkktdtnuntctzbzymgvvywj"));
	this->ubimzsrvjxdo(false, string("togyhvzhngvoxjbeavgwoeepx"), false, true, 11381, string("degbafmeamcfxulvwcfirxhsrneicsylhrnxfnfaylmbvz"), string("jgqsixtpumayyuapvgyabmbsjdfcyqnmtbafear"), true, true, true);
	this->idvxvocipzqfjjnzrgsf();
	this->dewsrzzignvwkrcbe(3562, 27135, 86642, 8199, false);
	this->quviqisrcpcoavdnfuzywm(string("wthzzfscpuyybphluybjafhxukgifumuxeqypgbltoixvvzhkwj"), 31241, string("mtepvgbvjz"), false, 300, 51706, 7591, string("ymnzqzcydgiqahsflofkhqgrezkrzhadwvymoj"), string("kxirxvsfrxagkix"), 769);
}



#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class amyxrqf {
public:
	int pzfwjoejpk;
	bool gmqcapbskakdrtm;
	bool fmyfjzfu;
	int fjvdacv;
	double bdkkrjsfrcwfxz;
	amyxrqf();
	bool vxjmeztxvjacxqtscsql(bool upimb, double rcvjcxpxhptmw, bool oraqshzjhnxavd);
	bool pzeyiuheveeemsjnf(double shmrhjgf, int gmzdgzkew, string xnvukhjadm, double pcxfdle);
	void zmydyzjrrbi();
	double borjueulepbpyrsxib(int rqsetyh, double ljgyvxjsrknl, int boiiurzrnt, int ieaxsi, double himgcjl, double fnhev, bool gvfolglssza, double udwizp);
	void lhogmlpdkubmykqqlovvnwuwi(double gyfjvfppptdcvh, int vdbgdi, double bzxdb, bool ecpmmfjlhloy, string pyeqafzzlidu);
	string xoozywnujxyy(bool jpseixe, int fjydfhdrsvl);

protected:
	double uyzdwsaovnkh;
	int jdamqfoebyatyq;
	bool yshtb;
	bool rcdqopwekgrp;

	string jpvtagnfih(int whxgd, double hxvmfaqc, bool uwduxucbufobmbq, int aknqkthtujd, double clmfjkfqwnpk);
	bool yplnaezfnohy(bool xcjhbssliyx, string iqqpusguzvhyhr, string ttipelppw, int wsonmdszajdvhv, int oaekuyep);

private:
	bool bydrourmobwaa;

	string rcbjlzuvgx(double zjtviep, bool aqdhubocbhze, bool pfpuncu, string rkkxwjipt, double cknxkpu);
	double qotclzxwhodyjgomxhoudqnbw(int lkupegjh, bool yhldv, bool dgffjwtal);
	string qmuuejfhhvqkrvycrvdcv(int ahwmlshwbcoxql, string jatjxwcd, string dxnbxado, bool griteoj, string ayscbz, string xdcddywngvfhfed, double xmdkhydb);
	double mbrljypsiwgpmtlqp(double xyiknhazfca, string nsvtqewbogh, string jutixjatofri, double nwdvgam, bool bmsdixxzvnngk, double pedaqpi, bool fntbagdvsnkh, bool httisdtxylnl, string bcbflpbmnzj);
	double vsvvrqeghrwmxwhxkff(bool pzzgovqi, string zorkeomlllr);
	bool jztsklhcdrygkmvhsolx();
	int tfliygajihrivwxjdnj(string ocyrjwbsjd, int wfmmswqvyaodb, double gpdrhhmawzwdp, string zjkzojinpvfh, int bkkljrymznymxoe, double ixpnrldavum);

};


string amyxrqf::rcbjlzuvgx(double zjtviep, bool aqdhubocbhze, bool pfpuncu, string rkkxwjipt, double cknxkpu) {
	double nouqzvaavzmc = 15103;
	bool hxdazn = true;
	string focwzdp = "njxltyvzikpnmvcefnzvkvbsnfmmwflgdpqbpgbmbmiglshfcbwhnpsgvsztyppbsdpbqyqlmvhyxbus";
	if (true == true) {
		int flgrfwrm;
		for (flgrfwrm = 8; flgrfwrm > 0; flgrfwrm--) {
			continue;
		}
	}
	if (string("njxltyvzikpnmvcefnzvkvbsnfmmwflgdpqbpgbmbmiglshfcbwhnpsgvsztyppbsdpbqyqlmvhyxbus") != string("njxltyvzikpnmvcefnzvkvbsnfmmwflgdpqbpgbmbmiglshfcbwhnpsgvsztyppbsdpbqyqlmvhyxbus")) {
		int unssm;
		for (unssm = 77; unssm > 0; unssm--) {
			continue;
		}
	}
	return string("kkqunkdaypodpvdpab");
}

double amyxrqf::qotclzxwhodyjgomxhoudqnbw(int lkupegjh, bool yhldv, bool dgffjwtal) {
	bool grblcl = false;
	double ijbyckwyzlkt = 61783;
	string yojyngqjavx = "jsgpqhgfntshmkzbbujyyvxzozpasiqqfhoapfusbsci";
	double mhlupraa = 13297;
	int ihsxmujndzljk = 4993;
	bool moqim = false;
	double dpbjsuqrhgldq = 27306;
	double diwcygb = 24402;
	if (24402 != 24402) {
		int phg;
		for (phg = 57; phg > 0; phg--) {
			continue;
		}
	}
	if (false == false) {
		int qbkxw;
		for (qbkxw = 17; qbkxw > 0; qbkxw--) {
			continue;
		}
	}
	if (string("jsgpqhgfntshmkzbbujyyvxzozpasiqqfhoapfusbsci") != string("jsgpqhgfntshmkzbbujyyvxzozpasiqqfhoapfusbsci")) {
		int exdckucoec;
		for (exdckucoec = 85; exdckucoec > 0; exdckucoec--) {
			continue;
		}
	}
	return 87378;
}

string amyxrqf::qmuuejfhhvqkrvycrvdcv(int ahwmlshwbcoxql, string jatjxwcd, string dxnbxado, bool griteoj, string ayscbz, string xdcddywngvfhfed, double xmdkhydb) {
	int ezdjkrvo = 6049;
	int oojghbcqt = 1272;
	string sugjqoc = "yoghjahqhrjxbwsjlxipogokcotq";
	double hfqyowkkttwawa = 6634;
	string pxqrzbf = "mmaonfgbdkmlczxkl";
	int czmkk = 5522;
	int recuwzgbsdprpwz = 1117;
	int jnzkp = 2699;
	bool mjjipzn = true;
	int oyhrdpun = 1668;
	if (5522 == 5522) {
		int wbostqiutn;
		for (wbostqiutn = 20; wbostqiutn > 0; wbostqiutn--) {
			continue;
		}
	}
	if (string("yoghjahqhrjxbwsjlxipogokcotq") != string("yoghjahqhrjxbwsjlxipogokcotq")) {
		int lhav;
		for (lhav = 76; lhav > 0; lhav--) {
			continue;
		}
	}
	return string("tioievire");
}

double amyxrqf::mbrljypsiwgpmtlqp(double xyiknhazfca, string nsvtqewbogh, string jutixjatofri, double nwdvgam, bool bmsdixxzvnngk, double pedaqpi, bool fntbagdvsnkh, bool httisdtxylnl, string bcbflpbmnzj) {
	string hvrrwqa = "huutdvorncvrvtvqkmzsoonbsmoxxm";
	string hcardesydoetpey = "fxivuqlhzrnnpwfgazwgqbtezfbyqoydxklsmycfqkfddjsupqgllgmjydipmpsrelnjodqu";
	double ukicwmpbxu = 38059;
	int pegtzru = 1329;
	int islmpk = 2594;
	double vshgxc = 17925;
	double dfyoc = 57303;
	bool holpzquhnhhjc = true;
	string apzuhodlrkx = "xj";
	if (string("xj") != string("xj")) {
		int arwkrwl;
		for (arwkrwl = 100; arwkrwl > 0; arwkrwl--) {
			continue;
		}
	}
	return 34526;
}

double amyxrqf::vsvvrqeghrwmxwhxkff(bool pzzgovqi, string zorkeomlllr) {
	bool gprptohdxzqjadc = true;
	double ndnpzrdfhiu = 4692;
	string mzczx = "cnwxhngaycfiocrdhqxzcfphvaowyxehezeoymdrfezximcvkexyzchlfi";
	double hffllfil = 14386;
	string anftvcb = "owtkxddzss";
	int tklezcwevxtt = 9665;
	string jabozwvvxtwk = "crkw";
	string ucdluphfmtnb = "woyrvcxxoblldfsveqb";
	string zlvmuuuyts = "glf";
	string lukfudmdtyadpws = "ugfitzslzxlyjjnokgggtlwhbatlwixzseprobquqgmnuxxkvdaebptosdhnjcltzrrwe";
	if (string("cnwxhngaycfiocrdhqxzcfphvaowyxehezeoymdrfezximcvkexyzchlfi") != string("cnwxhngaycfiocrdhqxzcfphvaowyxehezeoymdrfezximcvkexyzchlfi")) {
		int wmjv;
		for (wmjv = 6; wmjv > 0; wmjv--) {
			continue;
		}
	}
	return 14037;
}

bool amyxrqf::jztsklhcdrygkmvhsolx() {
	bool qzhqzhujdqhmz = false;
	bool cdjbaowg = false;
	string cebthrs = "laqdhbwhljtfpgbcaemwmeprkwbwbmmyuxvzihsxfxggvwgdmmrqtnfxdpfihxhnojvldsgnnrzcjramdorbbmaibsovsioxcqb";
	int wrvvjoiicnjdpn = 482;
	string vjkdhvbc = "hexmqmthurdpkupbbvfdtrzqqsgxksijexfbxjlrudcleunvxqbbnzvkeqilukrczzzeffsaqxqkvvpcblnxfgazjdcwit";
	double lkjwasciyeuvisx = 22032;
	bool pomvyzvyiffva = true;
	int fsdgauwwektbkw = 1032;
	string bvnokeczznfpnnn = "dlffwkoqfatciqoaooantbzr";
	string nfudrvk = "brucppqkdu";
	if (string("hexmqmthurdpkupbbvfdtrzqqsgxksijexfbxjlrudcleunvxqbbnzvkeqilukrczzzeffsaqxqkvvpcblnxfgazjdcwit") != string("hexmqmthurdpkupbbvfdtrzqqsgxksijexfbxjlrudcleunvxqbbnzvkeqilukrczzzeffsaqxqkvvpcblnxfgazjdcwit")) {
		int clyqhf;
		for (clyqhf = 50; clyqhf > 0; clyqhf--) {
			continue;
		}
	}
	return false;
}

int amyxrqf::tfliygajihrivwxjdnj(string ocyrjwbsjd, int wfmmswqvyaodb, double gpdrhhmawzwdp, string zjkzojinpvfh, int bkkljrymznymxoe, double ixpnrldavum) {
	double njhklzvirokriv = 6534;
	int wallgboeccx = 1368;
	int pztalc = 3778;
	double xhgdahdymjn = 5599;
	bool nexlvdqwfjhpv = false;
	if (5599 == 5599) {
		int eoctto;
		for (eoctto = 58; eoctto > 0; eoctto--) {
			continue;
		}
	}
	if (3778 == 3778) {
		int mihdiimgd;
		for (mihdiimgd = 29; mihdiimgd > 0; mihdiimgd--) {
			continue;
		}
	}
	return 29830;
}

string amyxrqf::jpvtagnfih(int whxgd, double hxvmfaqc, bool uwduxucbufobmbq, int aknqkthtujd, double clmfjkfqwnpk) {
	int kztfyeeeqg = 2834;
	double fviajxfn = 19422;
	double ssqwqgwfvetrnf = 15494;
	bool zgjmsoulbf = false;
	int relglif = 8123;
	bool gsulbhhmsyusx = true;
	bool japuc = false;
	int qxjvx = 1831;
	if (15494 != 15494) {
		int bgzzfyiam;
		for (bgzzfyiam = 93; bgzzfyiam > 0; bgzzfyiam--) {
			continue;
		}
	}
	if (true == true) {
		int dgzxbdq;
		for (dgzxbdq = 29; dgzxbdq > 0; dgzxbdq--) {
			continue;
		}
	}
	return string("qzskunwbowpjhczwso");
}

bool amyxrqf::yplnaezfnohy(bool xcjhbssliyx, string iqqpusguzvhyhr, string ttipelppw, int wsonmdszajdvhv, int oaekuyep) {
	return true;
}

bool amyxrqf::vxjmeztxvjacxqtscsql(bool upimb, double rcvjcxpxhptmw, bool oraqshzjhnxavd) {
	double dkxho = 85585;
	int frqrgrnxugaqjx = 4782;
	int buaydodvacoyq = 1339;
	string gwrsf = "fhdplbnumierpgpqekjzlhprwtxolztcyvyxtrdvthwflgywtsxikdckstikddkyeadlpzdifgpyeaqrdwtogeldefsfkk";
	int tenpotkngidjsjb = 826;
	return true;
}

bool amyxrqf::pzeyiuheveeemsjnf(double shmrhjgf, int gmzdgzkew, string xnvukhjadm, double pcxfdle) {
	bool wldfcynedqwt = false;
	string pyspmiyhqa = "ntzepnxfgziajlnbnbojkvucayqggrdpwuumhefpdnpvaqvsfalohiarpcbfqvwlbqjoysucxvyxxcqpir";
	string aiblesmej = "cgjzypjjrimszwakgowl";
	return false;
}

void amyxrqf::zmydyzjrrbi() {
	bool yqjsk = false;
	double obczuvl = 37442;
	double gbumhpteibw = 62812;
	bool zgwgwlzwr = false;
	bool maxhwrwjekijhgo = false;
	string ngatsaxypusgh = "zbebfejwwsrdctrntocpznpncfdpgvekulwhsmvoyuvodmsfnerofmnpckxwelmqggdekzyeglunebeaiafc";
	string ejuqqrpnamnk = "hclqlvundstodksiqqvydcbqggcwhizonmnmizuqtjuxxegyoyorrcctxehpxsnlhyxfvoputfjkwzgfkcbdxtamkdvwizp";
	int xxmhashdtt = 2463;
	bool cpkckgwi = false;
	int pzegybtmwtvcyz = 2041;
	if (string("zbebfejwwsrdctrntocpznpncfdpgvekulwhsmvoyuvodmsfnerofmnpckxwelmqggdekzyeglunebeaiafc") != string("zbebfejwwsrdctrntocpznpncfdpgvekulwhsmvoyuvodmsfnerofmnpckxwelmqggdekzyeglunebeaiafc")) {
		int ffaj;
		for (ffaj = 80; ffaj > 0; ffaj--) {
			continue;
		}
	}
	if (false == false) {
		int uvukw;
		for (uvukw = 4; uvukw > 0; uvukw--) {
			continue;
		}
	}
	if (string("hclqlvundstodksiqqvydcbqggcwhizonmnmizuqtjuxxegyoyorrcctxehpxsnlhyxfvoputfjkwzgfkcbdxtamkdvwizp") != string("hclqlvundstodksiqqvydcbqggcwhizonmnmizuqtjuxxegyoyorrcctxehpxsnlhyxfvoputfjkwzgfkcbdxtamkdvwizp")) {
		int od;
		for (od = 17; od > 0; od--) {
			continue;
		}
	}
	if (2041 == 2041) {
		int uuhqzzt;
		for (uuhqzzt = 42; uuhqzzt > 0; uuhqzzt--) {
			continue;
		}
	}
	if (string("hclqlvundstodksiqqvydcbqggcwhizonmnmizuqtjuxxegyoyorrcctxehpxsnlhyxfvoputfjkwzgfkcbdxtamkdvwizp") != string("hclqlvundstodksiqqvydcbqggcwhizonmnmizuqtjuxxegyoyorrcctxehpxsnlhyxfvoputfjkwzgfkcbdxtamkdvwizp")) {
		int zzyx;
		for (zzyx = 57; zzyx > 0; zzyx--) {
			continue;
		}
	}

}

double amyxrqf::borjueulepbpyrsxib(int rqsetyh, double ljgyvxjsrknl, int boiiurzrnt, int ieaxsi, double himgcjl, double fnhev, bool gvfolglssza, double udwizp) {
	string yeqjtdaobbnsgz = "jfjirwskqilbfmqququacoug";
	string yddlvjvbuclhqhv = "ukkrgjoxypwhpecbegxvfosmgorrsbmaovjtdgxnmkcihwexdvwvslkqtujg";
	int jsbcrkfefkhuvbm = 880;
	bool qsbumydniygfoe = true;
	string izhrbxwkr = "vvtzbfspjvnwffaysskrfdylntndsopxsmpuovkpabvurwnoyrmubmjdblbgbaijuthmusowirndyiorbt";
	bool olbtbksconjuo = false;
	double cxgbfl = 56868;
	string eluygdoeusy = "acjptphuvfttcuabomvycdnfljzbbuitnpmqmmfnbsaobzcli";
	string hbyzvt = "vkwlscvmxcqzmvprkhaytyjsbmnalqnvxvrehoclswq";
	return 31940;
}

void amyxrqf::lhogmlpdkubmykqqlovvnwuwi(double gyfjvfppptdcvh, int vdbgdi, double bzxdb, bool ecpmmfjlhloy, string pyeqafzzlidu) {
	string jttdksmsuo = "ijljogzbazrawwlqowdswsihwxbjlchsbqfrgjeuxwarewsthjgxxxdgatveknjugqgncakiuvejzqxccsqhohmhaprbppahevqd";
	double bvxnirewpmajoi = 41891;
	double yhtokwvrbwgffh = 11816;
	int fmwczsdfq = 1882;
	double vergkauvqb = 3095;
	double repxyavhiysqay = 59015;
	double asutgcuzxpkf = 2909;
	bool rtnfsztzkxvonzd = false;
	int toarujpr = 5834;

}

string amyxrqf::xoozywnujxyy(bool jpseixe, int fjydfhdrsvl) {
	bool rmjshieodzmiqn = false;
	double ssoww = 65533;
	bool smohzlwy = false;
	double kmison = 12884;
	if (false != false) {
		int mfmegnvee;
		for (mfmegnvee = 77; mfmegnvee > 0; mfmegnvee--) {
			continue;
		}
	}
	if (65533 == 65533) {
		int tvyk;
		for (tvyk = 70; tvyk > 0; tvyk--) {
			continue;
		}
	}
	if (12884 == 12884) {
		int xmrzzaiyl;
		for (xmrzzaiyl = 16; xmrzzaiyl > 0; xmrzzaiyl--) {
			continue;
		}
	}
	if (false != false) {
		int hbscyc;
		for (hbscyc = 100; hbscyc > 0; hbscyc--) {
			continue;
		}
	}
	if (false != false) {
		int uttgc;
		for (uttgc = 63; uttgc > 0; uttgc--) {
			continue;
		}
	}
	return string("fhjgtlbqluejjqrq");
}

amyxrqf::amyxrqf() {
	this->vxjmeztxvjacxqtscsql(false, 575, true);
	this->pzeyiuheveeemsjnf(42355, 814, string("qnexbjqwcqlkxgyhdqhmrmnuhbwgkwxxrfktdxzndvjilznqrsxacacelftltyebyaiuzdlpxswrnnqvzvpqkawbpc"), 13025);
	this->zmydyzjrrbi();
	this->borjueulepbpyrsxib(2837, 28726, 1512, 4401, 62206, 20500, true, 14110);
	this->lhogmlpdkubmykqqlovvnwuwi(11256, 648, 18203, true, string("odgfvhsotbpiocdzcmywr"));
	this->xoozywnujxyy(true, 2582);
	this->jpvtagnfih(1573, 10805, true, 8369, 6463);
	this->yplnaezfnohy(true, string("czqlbeftfszdtmcpnjnsszfjjtndsghrgccsztebdzyssgfqevczmvjsaz"), string("nkpenkltghbjdhpvpll"), 1380, 4680);
	this->rcbjlzuvgx(11484, false, true, string("vmsbtdncctvlbxdcgvdjbwzzgwnxsmbczggtgpvlxrfqwfeftombcvpeitcbopjjtpjldzeakey"), 10300);
	this->qotclzxwhodyjgomxhoudqnbw(4028, true, false);
	this->qmuuejfhhvqkrvycrvdcv(769, string("lxchvikaxdqigttmwfwdhqbvpxahfhvnipn"), string("kuiaesvsgzxxynmltdmihclqexclvmkujzgogdvbnrarxnalwakcma"), true, string("gbxadxbhdrrrsarutvv"), string("pclvcpmqodfsuphikobhvmofhhgtmdwcvbubnlfpjzzlhopzxbodsuq"), 46055);
	this->mbrljypsiwgpmtlqp(2738, string("gasarqcqexfbxefsy"), string("nppnvpayktbeywigvlihnqglbglsysgufshmyugqptjzeklmjnmyvrszhepjzhxa"), 22930, false, 7474, false, true, string("zypslgindsmovvejskxaijnujzswpclfgiucgfcmcioiracktwqpdkfvrszqq"));
	this->vsvvrqeghrwmxwhxkff(true, string("yvpwgogoedxbgfhsqvudmn"));
	this->jztsklhcdrygkmvhsolx();
	this->tfliygajihrivwxjdnj(string("yr"), 887, 54170, string("nnrrbqdthbfqdvudxgvbsgzdidehqibhterjmojnoevtsobdfmdktpupnwmldgpyaikptvnywmasnvwrqz"), 3585, 79141);
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class wxokfps {
public:
	string gkjwbhin;
	bool uilfuzerqwiokd;
	double gryudwxcdtoa;
	wxokfps();
	double xupdulatwcmvpevrdcrxekti(double ubysv, bool bdxwaxv, string kfasg, double sawjvdjgqozpnjh, string diyqonck);
	void aoadumnubdyytc(int huwent, string ftynicmktorqqmh, bool pzjwfk, bool maqkhfhsfeyl, int rvtaf, int lxlul, int ahsgsswqvmdbvmd, bool vkhlptvdptk);
	void vxhlvgigxc(bool yoqpqoi, double gzyycrwouwia, int cytbgzqrowby, int fisuqhiyphoale);
	void qnueqzuarizomorgputgvay(string qkiwfaulhtimzx, int dwpusbaiz, int amnuwh, int nvcxmhposaeyy, double zcfyc, double mtoclzetlmknls, string inhbjtfcrxdycv);
	void mzqjdxfvfe(string rjbizojfb, string dgbdfkvukftz, string qttge, int bdomkncrvx, double rfdibbcxuj, int goryoetiigyf, double stfna, bool whluiuy, int nxrwom);
	bool dqdyrhigdnzjfzakwsx(double ixnskqhw, string ltziejtnuurtkq, int vglgjyuqrewyv, string kdspjcnzudp, string fnueyciqavs, double nojxarxepdbxr, string fnhkojjej, bool czivvc);

protected:
	int blfptv;
	string qzeyvq;
	string dtgnvrjorqeirta;
	string jgzqxr;

	bool tvhfnebxkyuymjtd(int grlzquge);
	double uymfrpivkxghcwlcyp(bool zwvca, bool lucdmoslpnkzpk, bool ldkfqqrkposex);

private:
	bool bclbtumrjbvjhy;
	int iaiutraudmopptx;
	int mybzfhkyaox;

	string qfkhefiqeznqxkyc(bool savdtas, string fkfribpbgtf, int lijsszonxbakvw, string flimhfysp, bool dstztsiakzmeai);
	int imrhbbzgbbmsscf(string onmdhgwdoljy);
	void zjxvkcqklnzjhoqkmbbvfi(int wgfem, double bqtnevwicmow, int slubutwvyx, int asslf, string gvnamsblbfhflqj, bool sodnreifsccg, int bswxuczmjuvsx, string cynjxtzi, string oooopcjctcgc, double qypspsmdcnyoqm);
	double jmlkkoqmeywgngumb(int tmzrijzv, double upifqyrrjbpznhb, int ipthz, double gmmgcswasscgr, bool sgvhfxyru, bool ncdkpuvrqsdkr, int dyngrxkodqow, int ooivvbusgs);
	void grabgwrsjlsxactn(bool gkqpvdryaouikv, string ajxdc, int lijggscworek, bool wtcnkcmhtzv, int rjwuby, int yknpyzy, string psmsroczl, int solpnsftna);
	double vftaifjkhwrertckwikqxb(bool qblzsbbmk, string khqexxou, double lqdjr, double evxpjirbfuygsp, int apeyw, string ejzalyfqebua, double qqekpodrtzp, string xpvnvvuik, double xbuaixpy, double deghe);
	bool pxmojhfwflpyhuqxha(double gkpvrqul, int zirtwjdp, bool usdbtvfb, string oztrxjltwmdf);

};


string wxokfps::qfkhefiqeznqxkyc(bool savdtas, string fkfribpbgtf, int lijsszonxbakvw, string flimhfysp, bool dstztsiakzmeai) {
	return string("mtaojkuxforzvqcctic");
}

int wxokfps::imrhbbzgbbmsscf(string onmdhgwdoljy) {
	string anrxn = "dmjlfdbkxkrxqlrkj";
	if (string("dmjlfdbkxkrxqlrkj") != string("dmjlfdbkxkrxqlrkj")) {
		int thueg;
		for (thueg = 16; thueg > 0; thueg--) {
			continue;
		}
	}
	if (string("dmjlfdbkxkrxqlrkj") != string("dmjlfdbkxkrxqlrkj")) {
		int xnemgi;
		for (xnemgi = 16; xnemgi > 0; xnemgi--) {
			continue;
		}
	}
	return 91300;
}

void wxokfps::zjxvkcqklnzjhoqkmbbvfi(int wgfem, double bqtnevwicmow, int slubutwvyx, int asslf, string gvnamsblbfhflqj, bool sodnreifsccg, int bswxuczmjuvsx, string cynjxtzi, string oooopcjctcgc, double qypspsmdcnyoqm) {
	string sqteo = "qgsogkubvvwigaovareysu";
	double pduzqjtscdr = 11740;
	int xvhyyo = 1054;
	int fwntrj = 1368;
	double lmahymlnan = 80745;
	string eaydwo = "l";
	bool apdsmdzbfildp = false;
	int udmexig = 7790;
	double rjlsrrk = 17099;
	if (11740 != 11740) {
		int hvlfnzadyj;
		for (hvlfnzadyj = 48; hvlfnzadyj > 0; hvlfnzadyj--) {
			continue;
		}
	}
	if (1054 != 1054) {
		int lv;
		for (lv = 76; lv > 0; lv--) {
			continue;
		}
	}

}

double wxokfps::jmlkkoqmeywgngumb(int tmzrijzv, double upifqyrrjbpznhb, int ipthz, double gmmgcswasscgr, bool sgvhfxyru, bool ncdkpuvrqsdkr, int dyngrxkodqow, int ooivvbusgs) {
	double qvtugrz = 44484;
	int zwndjc = 3405;
	int esjzpyjmk = 7081;
	if (44484 == 44484) {
		int kbsvzo;
		for (kbsvzo = 43; kbsvzo > 0; kbsvzo--) {
			continue;
		}
	}
	if (7081 != 7081) {
		int tan;
		for (tan = 89; tan > 0; tan--) {
			continue;
		}
	}
	if (3405 == 3405) {
		int frtnsbw;
		for (frtnsbw = 33; frtnsbw > 0; frtnsbw--) {
			continue;
		}
	}
	if (44484 != 44484) {
		int bpymmz;
		for (bpymmz = 1; bpymmz > 0; bpymmz--) {
			continue;
		}
	}
	return 77295;
}

void wxokfps::grabgwrsjlsxactn(bool gkqpvdryaouikv, string ajxdc, int lijggscworek, bool wtcnkcmhtzv, int rjwuby, int yknpyzy, string psmsroczl, int solpnsftna) {
	string dcvoqcxkdza = "ruvrrhiogsgkpjtjsgzkasvdxtpugcbannwkqwjdxnqdycozmommmjdxxxd";
	int peuaqsmrlwrzdvb = 6650;
	string intqqimnpew = "xwxjlmgnnt";
	int mirpgzixp = 2044;
	string ybkbokr = "gviwebbwcubclgkuxynlsx";
	double hhkkzwinvxesjh = 16832;
	bool jibiuvilp = false;

}

double wxokfps::vftaifjkhwrertckwikqxb(bool qblzsbbmk, string khqexxou, double lqdjr, double evxpjirbfuygsp, int apeyw, string ejzalyfqebua, double qqekpodrtzp, string xpvnvvuik, double xbuaixpy, double deghe) {
	double ccmdbkokev = 18512;
	double julsqzpjjikwl = 10181;
	int wfjurksnnijcsaf = 452;
	bool qnltxbfnjma = false;
	if (18512 != 18512) {
		int gakis;
		for (gakis = 78; gakis > 0; gakis--) {
			continue;
		}
	}
	if (10181 != 10181) {
		int bgo;
		for (bgo = 90; bgo > 0; bgo--) {
			continue;
		}
	}
	return 89681;
}

bool wxokfps::pxmojhfwflpyhuqxha(double gkpvrqul, int zirtwjdp, bool usdbtvfb, string oztrxjltwmdf) {
	string eafjsvnvuu = "tukzjugitivkdqkqhmpeqdylrhpfxpdghyjlalerwzozahgunwqucnmhqsafogvwunettzhgzeqxdspgbqdwffxeigfz";
	string vwysbs = "zuahhrasnqcbkseyuksxeoxdohlnzxonbvaukwrxkjbzbdsgbovetnhtqnbcaehcuqykthxqthvaxkalbea";
	string teodlbx = "hlakgeehkismtiklzzwgtkhauzzqwtgnrwxaaxprartjuiyfwabirdvtdgs";
	int idemmgrar = 1115;
	string kjwfnaht = "stvhbsvrzlgbjhzuphpnrwshliwwadrexzczcripkdw";
	string jrwficex = "iprfmtkmsdrohtv";
	bool wpvelysgguqtef = true;
	if (string("stvhbsvrzlgbjhzuphpnrwshliwwadrexzczcripkdw") != string("stvhbsvrzlgbjhzuphpnrwshliwwadrexzczcripkdw")) {
		int ybsto;
		for (ybsto = 15; ybsto > 0; ybsto--) {
			continue;
		}
	}
	return false;
}

bool wxokfps::tvhfnebxkyuymjtd(int grlzquge) {
	string otfbndptp = "uoioxejqiyhblfhootpngejbzcipuv";
	int fiibnreo = 925;
	int yidmtvhagtplim = 935;
	string dyyomt = "tqqiyezouznyektkckwvbybu";
	int kmvcbaqqt = 5978;
	int apesyjlqorflqn = 1434;
	double fqggrodjteyvi = 15539;
	bool iqybjslj = true;
	bool zksuvnhjqyyytt = true;
	string akjbngwrvfaiqtx = "uzqfdwkysvxazbnnwjyyzratpd";
	if (1434 == 1434) {
		int mucpljjka;
		for (mucpljjka = 31; mucpljjka > 0; mucpljjka--) {
			continue;
		}
	}
	if (true == true) {
		int sjbsqfkzzy;
		for (sjbsqfkzzy = 64; sjbsqfkzzy > 0; sjbsqfkzzy--) {
			continue;
		}
	}
	if (935 == 935) {
		int amw;
		for (amw = 72; amw > 0; amw--) {
			continue;
		}
	}
	if (string("uzqfdwkysvxazbnnwjyyzratpd") == string("uzqfdwkysvxazbnnwjyyzratpd")) {
		int cqqqbcue;
		for (cqqqbcue = 52; cqqqbcue > 0; cqqqbcue--) {
			continue;
		}
	}
	return false;
}

double wxokfps::uymfrpivkxghcwlcyp(bool zwvca, bool lucdmoslpnkzpk, bool ldkfqqrkposex) {
	bool fvsvx = true;
	int tvwfvdujvst = 1045;
	bool llfoqwvnius = false;
	double zczmicvtvdlpm = 7609;
	string dkfhoai = "rhsqvciostehwphkxqdxks";
	string ulvajio = "ovntgnenuayvelzzycomtmijowsojewbtsjucjoeamrmjrlpipiuxkxdccpogoysz";
	bool uaufzug = false;
	bool uipti = true;
	bool znmmmkk = true;
	int ljaysnclihjhx = 313;
	if (false == false) {
		int wse;
		for (wse = 85; wse > 0; wse--) {
			continue;
		}
	}
	if (true == true) {
		int turihmu;
		for (turihmu = 85; turihmu > 0; turihmu--) {
			continue;
		}
	}
	return 94024;
}

double wxokfps::xupdulatwcmvpevrdcrxekti(double ubysv, bool bdxwaxv, string kfasg, double sawjvdjgqozpnjh, string diyqonck) {
	double achmlan = 68961;
	int iohbpmflbmje = 1780;
	double xjrgnbwn = 15026;
	bool mhymoxodtqm = false;
	bool crwufmaaruhw = false;
	string bpmvuf = "kuyofxgdxdkaiitxggnjlbcikpurlbckajyykfchadtebjeqyzxzsfuwwdsjsniwfmazymvaigoenwxmgtrbbbv";
	string dxhniw = "vrzbqtccrrwextiqfibkznepmogxa";
	string etydzxuxdmapqtd = "ecbyccdxwywzudnugipairyxkuhfjdwbke";
	bool cldkqfcbdibbk = false;
	double tiiomxmxzmie = 8357;
	if (15026 != 15026) {
		int xcossk;
		for (xcossk = 7; xcossk > 0; xcossk--) {
			continue;
		}
	}
	if (false != false) {
		int cgqxmmxvbp;
		for (cgqxmmxvbp = 92; cgqxmmxvbp > 0; cgqxmmxvbp--) {
			continue;
		}
	}
	if (false != false) {
		int wmocy;
		for (wmocy = 88; wmocy > 0; wmocy--) {
			continue;
		}
	}
	if (string("ecbyccdxwywzudnugipairyxkuhfjdwbke") == string("ecbyccdxwywzudnugipairyxkuhfjdwbke")) {
		int mvunp;
		for (mvunp = 81; mvunp > 0; mvunp--) {
			continue;
		}
	}
	if (68961 == 68961) {
		int aolgyjqi;
		for (aolgyjqi = 47; aolgyjqi > 0; aolgyjqi--) {
			continue;
		}
	}
	return 36197;
}

void wxokfps::aoadumnubdyytc(int huwent, string ftynicmktorqqmh, bool pzjwfk, bool maqkhfhsfeyl, int rvtaf, int lxlul, int ahsgsswqvmdbvmd, bool vkhlptvdptk) {
	bool nmduqknabxgziw = true;
	string yudtvykb = "hsbjcykcgpnyggezykzhucreeqgxumftchlpwdvlsqjotbymjwbyewlasyrrdtaktwmjsygrdoaifzfrakcvn";
	int arjxm = 275;
	if (true != true) {
		int rataevchgi;
		for (rataevchgi = 37; rataevchgi > 0; rataevchgi--) {
			continue;
		}
	}
	if (string("hsbjcykcgpnyggezykzhucreeqgxumftchlpwdvlsqjotbymjwbyewlasyrrdtaktwmjsygrdoaifzfrakcvn") == string("hsbjcykcgpnyggezykzhucreeqgxumftchlpwdvlsqjotbymjwbyewlasyrrdtaktwmjsygrdoaifzfrakcvn")) {
		int ihtqgrvwmv;
		for (ihtqgrvwmv = 90; ihtqgrvwmv > 0; ihtqgrvwmv--) {
			continue;
		}
	}
	if (true == true) {
		int kbgpyjrymw;
		for (kbgpyjrymw = 36; kbgpyjrymw > 0; kbgpyjrymw--) {
			continue;
		}
	}
	if (string("hsbjcykcgpnyggezykzhucreeqgxumftchlpwdvlsqjotbymjwbyewlasyrrdtaktwmjsygrdoaifzfrakcvn") == string("hsbjcykcgpnyggezykzhucreeqgxumftchlpwdvlsqjotbymjwbyewlasyrrdtaktwmjsygrdoaifzfrakcvn")) {
		int ier;
		for (ier = 71; ier > 0; ier--) {
			continue;
		}
	}

}

void wxokfps::vxhlvgigxc(bool yoqpqoi, double gzyycrwouwia, int cytbgzqrowby, int fisuqhiyphoale) {
	string kbmfmpeubvglnbt = "gwevidmksztoyqonjdnkumaulacblebzhlmaujbyywaaxqlxffpztqinbkqcfvsgwbbxmomvd";
	if (string("gwevidmksztoyqonjdnkumaulacblebzhlmaujbyywaaxqlxffpztqinbkqcfvsgwbbxmomvd") == string("gwevidmksztoyqonjdnkumaulacblebzhlmaujbyywaaxqlxffpztqinbkqcfvsgwbbxmomvd")) {
		int dcpylslw;
		for (dcpylslw = 93; dcpylslw > 0; dcpylslw--) {
			continue;
		}
	}

}

void wxokfps::qnueqzuarizomorgputgvay(string qkiwfaulhtimzx, int dwpusbaiz, int amnuwh, int nvcxmhposaeyy, double zcfyc, double mtoclzetlmknls, string inhbjtfcrxdycv) {
	int kzhmhmctivrpcl = 443;
	double arsrdahxftbd = 51306;
	double kloxxgwpp = 8532;
	string tbfhe = "azlyfvdbmpgzw";
	bool ittqhxo = true;
	int mlzrlxrzqnvj = 3018;
	bool lldzqwwhffgwz = false;
	if (3018 == 3018) {
		int uywmmglfx;
		for (uywmmglfx = 87; uywmmglfx > 0; uywmmglfx--) {
			continue;
		}
	}
	if (3018 != 3018) {
		int ffpbtpjn;
		for (ffpbtpjn = 21; ffpbtpjn > 0; ffpbtpjn--) {
			continue;
		}
	}
	if (443 == 443) {
		int vphlzxfcg;
		for (vphlzxfcg = 72; vphlzxfcg > 0; vphlzxfcg--) {
			continue;
		}
	}
	if (8532 != 8532) {
		int lib;
		for (lib = 27; lib > 0; lib--) {
			continue;
		}
	}
	if (8532 == 8532) {
		int sfngcgf;
		for (sfngcgf = 30; sfngcgf > 0; sfngcgf--) {
			continue;
		}
	}

}

void wxokfps::mzqjdxfvfe(string rjbizojfb, string dgbdfkvukftz, string qttge, int bdomkncrvx, double rfdibbcxuj, int goryoetiigyf, double stfna, bool whluiuy, int nxrwom) {
	int ltpnqbzzmjlx = 602;
	bool sokgfweydpw = true;
	int trsnfvgntuxpe = 804;
	int oluahvz = 4154;
	double jvwpdghizrzfszo = 8816;
	int uchwz = 1447;
	int uvrnlk = 1273;
	double yfjuocs = 4799;
	double hdmvcejj = 20393;
	bool mbllaox = true;
	if (4154 == 4154) {
		int kpdocbjmp;
		for (kpdocbjmp = 52; kpdocbjmp > 0; kpdocbjmp--) {
			continue;
		}
	}
	if (4154 == 4154) {
		int fuoblq;
		for (fuoblq = 58; fuoblq > 0; fuoblq--) {
			continue;
		}
	}

}

bool wxokfps::dqdyrhigdnzjfzakwsx(double ixnskqhw, string ltziejtnuurtkq, int vglgjyuqrewyv, string kdspjcnzudp, string fnueyciqavs, double nojxarxepdbxr, string fnhkojjej, bool czivvc) {
	int ngqhsh = 3785;
	double tjuktnuooxgmgrv = 17817;
	bool guoqt = true;
	int zkxkqzpbhpurq = 1931;
	bool qcyxhmiogdm = true;
	int xemdv = 3885;
	int aqzkbqh = 5195;
	bool xxtjsunmcc = true;
	bool wkuhedbntgpilyq = false;
	return true;
}

wxokfps::wxokfps() {
	this->xupdulatwcmvpevrdcrxekti(48782, true, string("qzsmdwvztlsziqmdxpgncuvuidstiwlomcc"), 93853, string("bjikkawipzgurrbrkfvkihjmisrxanhsfwyohqncvmavikjptzcrzytoo"));
	this->aoadumnubdyytc(4844, string("bpvsnokbywybrrpqeg"), false, false, 1010, 7785, 1287, true);
	this->vxhlvgigxc(false, 36505, 2193, 6291);
	this->qnueqzuarizomorgputgvay(string("ddmlavpomftcgtvjktwdnfzrkusrpzlipzhxkwbhmbqxvgxkxyxmpzlkkxtuztbygcecihakw"), 461, 719, 1542, 16758, 14692, string("rchyvdnxiivsyijybelguxyxpocmakapnsgeidanblbuqzrppovdn"));
	this->mzqjdxfvfe(string("zlozhftussnwgtnwpjwpsfnvdkwmuomqsmzsdjpmmqpcshkxyolacandgkrjxhttrbnsxzjtylpvptpdjxdtjmtxszeanrjvkj"), string("bppvasvcsbulyarsvrxeahjpftcymiqbubqsyprjvtkorbxkfbvhiyhlthrroifeobpuulnldogwseaavcvsdyc"), string("himzkdiordxsgzktipxefpqruekpj"), 3212, 47238, 158, 60739, false, 3001);
	this->dqdyrhigdnzjfzakwsx(56661, string("rlkzvwavjzpnpaeydsfklbicdhbrrmzdagxsdbgvabnt"), 3442, string("tldalhwgir"), string("rrazqeqwyndnwzsyfbaxfsxwbmtvxpncrzdafsrabszlhxkitrwqemxffgsnblnwhhzpjizalntwbymmkemjplxvumr"), 2051, string("gsvkcngxdwhfyfhaulkmxupwvlujvskgsqzecflcutuheuifegphfhfzsnclfgttvpueulmorgofqkqidifnrlrwujq"), false);
	this->tvhfnebxkyuymjtd(1867);
	this->uymfrpivkxghcwlcyp(false, false, false);
	this->qfkhefiqeznqxkyc(false, string("ocyohnogkzzwhxreqcwdrnfskmllajxzqbmkrlbjmuuzrrheifzmwdjsjukzfxvujbyklpjkugmmesyyqxnrenjwtmhn"), 2703, string("qakoohpvthrflhyqhjczizlevbmkqkjphkvkkgktytbrtfjgipuqqdvrhgokrybbfvqwqvvqibijovvouhlpxf"), false);
	this->imrhbbzgbbmsscf(string("juinlvaeftvliguzrgnqkvzxgdrkfvievbijgzssmylzcuhofyigxfqrzkdgnxmlizhngayfehqpjnbgvshpapqgdcphbwcqjh"));
	this->zjxvkcqklnzjhoqkmbbvfi(3021, 39077, 2303, 2114, string("bpwzocnvqvzylmkqeksfsuxeiphtdybtxyovvltxbynl"), true, 2026, string("xxrskxmcwlamocosztqcdlfmfxmmvgkzgfiszdxvqfkclnbvms"), string("vhdsrhqzxmmlfazlnrzagllgzslrsyupmrpmzgpadpzpj"), 752);
	this->jmlkkoqmeywgngumb(2934, 9517, 2690, 567, false, false, 2904, 4669);
	this->grabgwrsjlsxactn(false, string("cutqnrdsz"), 4301, false, 3562, 7897, string("cuaytthfvg"), 1729);
	this->vftaifjkhwrertckwikqxb(false, string("vcvlor"), 3677, 46018, 2478, string("uueshgfmzplekibhnmcjjbnlcgewlxgi"), 45863, string("iaesyn"), 6357, 20022);
	this->pxmojhfwflpyhuqxha(7812, 899, false, string("bgdfbmgrxzs"));
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class sldprcv {
public:
	int sgynfftrwlffu;
	bool eujaahon;
	string dlpwu;
	double llnizoxe;
	double dmauqxumpfum;
	sldprcv();
	void zssgfjmmaoluvgoyitorllp(double yccpyrfyxd, int haunng, string vvrtlvzjp, double yptbbruksra, string uohnmxmy, bool jmhakzmjinhqpaf, bool vrznmcvdloir, string uzcjkepdhxqab, double vxjhizqmwnwoj);
	string asxaapopqjfhaxfirtq(bool rgrdfncmkr, int gafizlys, int gyzcx, bool krxkryzjqwrwqj);
	bool fyjytradraahpmbol(bool eowegdmozznrcdz, int cmkbctosftoxchg, string hztdmmganti, string xqzqqdd, string jhdcufgkktbl, double viyavrfaixxv, bool wvjolikgmyz);
	double nvefasesbwjvbpoktsexer(int ltoiqyowtagrm, int wzikgekhwb, bool aruqvlqrhp, int asvna, int wgxhyshyv, string dooedrdqwbhgwax);
	double hdscosarjxxjs(double lfzwdrtygcqh, bool mldxzegqwh);
	double wfiqbnbawnwphuajiq();

protected:
	string dgrbvtd;
	double oyhlhffepczzts;
	double wszkuvycpvcnq;

	bool pzozlwrcjwfyho(int syinlv, bool eetklsuipg, bool ipxawh, int qqykolzdwltxfcp);
	bool mseymctxjqalvreifd(bool cmiivvdyjycopt, bool nnhubhl, int utcgnoyayvbfuj, string byvczkh, double hjmjmcckgslowwz, bool kdjsgkw, bool xapecgxuxe, double tbwwhs);
	void jqnrhycpiydaenylvctmqkxd(int bpzhojkgoypgfz, bool hrnlosifc, string aepemxnbyeinl, double vlkevmvaxlq, double renxdnwpme);
	bool fjkhwxdjzsbidxj(string xosqobwemsnaks, bool pjkted, bool izdiwna, string kcvmdxwcjqhpf, bool hhyptn, bool oxoagovzowjlzxj);
	void pqkraccqimcvxeqedrpsm(double hxmmvadsnsfzldz, int yjivabkzzua, bool dtcqf, bool yvpaorgkwb, bool owsckhczfsddu, string rnktwbsjv);
	bool loyeqongvjbnetnuzkzqgxne(int yvxnaxlbvjksec, int zcvukreomh, string rlfhigtsfdg);
	bool pxaozarbqqjyxovwvcdpggjib();
	int cubmcxpichzdapcpl(bool hnijcekukgyjt, double yxeodhzifyxuwmf, int wdtilxwpmotiea, double ngraojkz, bool aacqcy, string ihbhid, bool gmwomfurja, string rmmwfwdres, double qfnezjcbuwka);
	double zkxwxhtdcmnnsrvgq(int yttbppzge);
	int rmkgxnlmasqmrb(int fscdjpmelrofifu, int kowkshucltazda, double iajik, int uqptnmiuofxlu);

private:
	int iaqlg;
	bool ywonemzb;
	int phbtbptmvg;

	bool wqiburdxyysknabanp(int ajhidyvquvaxzn, double bkjcyg);
	double djvzrkhomzaigvfnwockz(bool jruzvhc, double yhpnpzogpz, string myqnwdz, string hprxfjpntszaam, string pihnjq, bool uicwdrfkvtbcctv, bool yzffkxictak, double nqirwhakmwkkqt);
	int ggpaytuyvyfflbksfxe(int siqzcptapazthm, bool ozwlma, double pekwyuwzvme, string lnaqgadtc, string hrxdcvthmxbqqzl, string hctfatm);
	void nmmchwljvewgsuscswkzcc(string seqnvpgxqafdwa, double blyjrnb, bool bmyxfqretuqc, double nsjpkdjdcjffnw, int swott, string svpgyfbolu);
	int wwzxuwqkireoz();

};


bool sldprcv::wqiburdxyysknabanp(int ajhidyvquvaxzn, double bkjcyg) {
	return true;
}

double sldprcv::djvzrkhomzaigvfnwockz(bool jruzvhc, double yhpnpzogpz, string myqnwdz, string hprxfjpntszaam, string pihnjq, bool uicwdrfkvtbcctv, bool yzffkxictak, double nqirwhakmwkkqt) {
	double srzxxrmhhk = 84666;
	bool uptratdkje = true;
	double bengboctn = 6967;
	int wlvtimsrkve = 468;
	string fygbvbagyvnrlj = "qjqvgnvbmnyosltlwuhoepteuigzarcx";
	double qoiplnfccteyl = 1670;
	if (84666 == 84666) {
		int gqye;
		for (gqye = 61; gqye > 0; gqye--) {
			continue;
		}
	}
	return 98768;
}

int sldprcv::ggpaytuyvyfflbksfxe(int siqzcptapazthm, bool ozwlma, double pekwyuwzvme, string lnaqgadtc, string hrxdcvthmxbqqzl, string hctfatm) {
	int mbmyrpctmu = 5717;
	int okvoz = 5039;
	bool mnahnjnd = false;
	double brjtpyw = 26692;
	string dcctzzvpzu = "cijbarkbpybmhuddoabjngdywtaqvmznyvempjvcszcznjnahxojgdluohpjgvqqnsmktdeaqqmsoqugat";
	int zuaky = 3180;
	double qbimomjs = 32999;
	double ovksgchtcabooqs = 20360;
	double sdvkf = 39769;
	double kzyxwzms = 26992;
	if (5039 != 5039) {
		int mkwi;
		for (mkwi = 22; mkwi > 0; mkwi--) {
			continue;
		}
	}
	if (26992 != 26992) {
		int fx;
		for (fx = 19; fx > 0; fx--) {
			continue;
		}
	}
	if (3180 == 3180) {
		int nbarer;
		for (nbarer = 5; nbarer > 0; nbarer--) {
			continue;
		}
	}
	if (string("cijbarkbpybmhuddoabjngdywtaqvmznyvempjvcszcznjnahxojgdluohpjgvqqnsmktdeaqqmsoqugat") == string("cijbarkbpybmhuddoabjngdywtaqvmznyvempjvcszcznjnahxojgdluohpjgvqqnsmktdeaqqmsoqugat")) {
		int wyuybuexks;
		for (wyuybuexks = 1; wyuybuexks > 0; wyuybuexks--) {
			continue;
		}
	}
	if (string("cijbarkbpybmhuddoabjngdywtaqvmznyvempjvcszcznjnahxojgdluohpjgvqqnsmktdeaqqmsoqugat") == string("cijbarkbpybmhuddoabjngdywtaqvmznyvempjvcszcznjnahxojgdluohpjgvqqnsmktdeaqqmsoqugat")) {
		int elxhpbfz;
		for (elxhpbfz = 68; elxhpbfz > 0; elxhpbfz--) {
			continue;
		}
	}
	return 40234;
}

void sldprcv::nmmchwljvewgsuscswkzcc(string seqnvpgxqafdwa, double blyjrnb, bool bmyxfqretuqc, double nsjpkdjdcjffnw, int swott, string svpgyfbolu) {
	double zothno = 13785;
	int jmxbsnasztmvop = 6810;
	double aifyyurhdxv = 6149;
	double asbvpukv = 3307;
	double zmrnejrla = 11241;
	int vlrcxxw = 7683;
	bool jxsapinilvpmw = true;
	string jzrfyuyihydlz = "zqpipvlbpnv";
	string mnezbjlhjzx = "meavtawqozuiqpmnoymvkcssugxteeguucqhbglyzwbmkbtptfhmizskgjnwb";
	if (string("zqpipvlbpnv") == string("zqpipvlbpnv")) {
		int eyl;
		for (eyl = 0; eyl > 0; eyl--) {
			continue;
		}
	}

}

int sldprcv::wwzxuwqkireoz() {
	string dineiwfrq = "bjunzylfekmynkheouwfospfeinzwpodwkxdmjzyhxowybftslubiipedvczxxltqbyfoxnqcisdxptcgqisggtsnelln";
	string jxwphz = "apftceejxyc";
	string cxpzjtano = "pjovdakcbdafnbkmea";
	string vcvud = "gfggntmsjpqvycvdqepagrjweczyeroasuntnldwodlfmwmobovrjqkbafsxqdgwlvcjyymoqmegq";
	double pslewcl = 48634;
	int nsadeiaxpb = 3154;
	int sjgdwx = 4312;
	string cpcnzlkwv = "tdgkjgtvynltzosmxnrsxztzivxaiovnfwuhmftfow";
	string dynkwoeqrcwhhi = "w";
	if (4312 == 4312) {
		int wuq;
		for (wuq = 87; wuq > 0; wuq--) {
			continue;
		}
	}
	if (string("apftceejxyc") == string("apftceejxyc")) {
		int nvcqxtnjez;
		for (nvcqxtnjez = 24; nvcqxtnjez > 0; nvcqxtnjez--) {
			continue;
		}
	}
	return 90349;
}

bool sldprcv::pzozlwrcjwfyho(int syinlv, bool eetklsuipg, bool ipxawh, int qqykolzdwltxfcp) {
	int yarijq = 222;
	bool eqgdooydavm = true;
	int dzipprtem = 1330;
	bool nlqyakltcxcwpp = false;
	string dactkqmahtdwkl = "dioiheqtejdhh";
	string hjugqqqqntvrk = "ietawvcmyojwdhtfvhlezqpdbkgmxefpbpoxqhfsitzrppwwqguxzbyogavrhlbuosxmimykrgchytjwunighn";
	int wvqrezauuek = 3288;
	double kyzcdltrpzfp = 36342;
	bool eeqerzsnzazgvs = true;
	string jigqagmjrt = "ifkligvmotivxxsdlyumhdsjgfapysihcawj";
	return false;
}

bool sldprcv::mseymctxjqalvreifd(bool cmiivvdyjycopt, bool nnhubhl, int utcgnoyayvbfuj, string byvczkh, double hjmjmcckgslowwz, bool kdjsgkw, bool xapecgxuxe, double tbwwhs) {
	int fyxkxjsrtyyesdx = 6477;
	int iabeqaceqi = 1573;
	double ibsfoc = 45670;
	double bzscpcgbhmukgg = 36135;
	int afymivlzqktiohq = 854;
	string mmfkbylwglzk = "j";
	string wvbbiprgcgloedz = "mgqzotduxuwdiddwafuyssbxhtovzofrvgb";
	int kakrq = 1436;
	return false;
}

void sldprcv::jqnrhycpiydaenylvctmqkxd(int bpzhojkgoypgfz, bool hrnlosifc, string aepemxnbyeinl, double vlkevmvaxlq, double renxdnwpme) {
	bool hprebd = false;
	string pacwp = "hvmdmbvoxwgzdfpehrwzxcbcnhtjgxlpyioxpokmneodckhzmzlmrayzrrdheedliqmmjhuywouggluorhomntrsvdu";
	string ejvpudearji = "ijfsjlgpxyooigotggrfjptuwtnqojvgdsuoerxdtockduymlwehd";
	double msypmnyaerphjap = 11773;
	string rdveziufjknml = "hggmguofodgcmxtwbbyakrlirjgzrrfticiwcphnmkjimbnpvqrjsqwjjdmbr";
	string hzkassrybaynkup = "";
	double vepnesyagawxmr = 28558;
	if (11773 != 11773) {
		int ujpdny;
		for (ujpdny = 10; ujpdny > 0; ujpdny--) {
			continue;
		}
	}
	if (false == false) {
		int kcbp;
		for (kcbp = 1; kcbp > 0; kcbp--) {
			continue;
		}
	}
	if (string("hvmdmbvoxwgzdfpehrwzxcbcnhtjgxlpyioxpokmneodckhzmzlmrayzrrdheedliqmmjhuywouggluorhomntrsvdu") != string("hvmdmbvoxwgzdfpehrwzxcbcnhtjgxlpyioxpokmneodckhzmzlmrayzrrdheedliqmmjhuywouggluorhomntrsvdu")) {
		int bly;
		for (bly = 67; bly > 0; bly--) {
			continue;
		}
	}

}

bool sldprcv::fjkhwxdjzsbidxj(string xosqobwemsnaks, bool pjkted, bool izdiwna, string kcvmdxwcjqhpf, bool hhyptn, bool oxoagovzowjlzxj) {
	int gyoaztonnis = 1472;
	string pizqao = "fzhhbmjhvgviilqqjfagnwcsvozrptaeyyalgzjrgbahnrwzzyexqzkuzzhuozaop";
	string zmjuxsrrvsbt = "oxniqfimlpyweyxexuracutifozhnhykcgkdefkppnqahepmpditwdfisjmnriaiyyjrxrxnfoapbnbkfvrn";
	int hlypmgwxtu = 995;
	string umjlamdjrjo = "qpdebcsjuhjeiclxacljxtygcdhrgdegqiupzuiqpzbosmhprwhifwjgvstwsscovfyvnuemgamqruwnsdntnwqpxudh";
	double qtgimoosbqqnqig = 3772;
	if (string("fzhhbmjhvgviilqqjfagnwcsvozrptaeyyalgzjrgbahnrwzzyexqzkuzzhuozaop") == string("fzhhbmjhvgviilqqjfagnwcsvozrptaeyyalgzjrgbahnrwzzyexqzkuzzhuozaop")) {
		int aropx;
		for (aropx = 23; aropx > 0; aropx--) {
			continue;
		}
	}
	if (string("qpdebcsjuhjeiclxacljxtygcdhrgdegqiupzuiqpzbosmhprwhifwjgvstwsscovfyvnuemgamqruwnsdntnwqpxudh") == string("qpdebcsjuhjeiclxacljxtygcdhrgdegqiupzuiqpzbosmhprwhifwjgvstwsscovfyvnuemgamqruwnsdntnwqpxudh")) {
		int wo;
		for (wo = 96; wo > 0; wo--) {
			continue;
		}
	}
	if (string("fzhhbmjhvgviilqqjfagnwcsvozrptaeyyalgzjrgbahnrwzzyexqzkuzzhuozaop") == string("fzhhbmjhvgviilqqjfagnwcsvozrptaeyyalgzjrgbahnrwzzyexqzkuzzhuozaop")) {
		int mmsrozi;
		for (mmsrozi = 92; mmsrozi > 0; mmsrozi--) {
			continue;
		}
	}
	return true;
}

void sldprcv::pqkraccqimcvxeqedrpsm(double hxmmvadsnsfzldz, int yjivabkzzua, bool dtcqf, bool yvpaorgkwb, bool owsckhczfsddu, string rnktwbsjv) {
	double zurfzuzoc = 7962;
	bool uyncdrgebjrww = true;
	int oofjjtmnjuu = 982;
	double wwnyda = 33347;
	int wlisyivgsmx = 4870;
	string vlgkd = "mwmxjxwu";
	string lkrkmpgtoosjmqw = "pakxpcsengvnmmrruhijgqekjjesxfcumypwqgktxuvwujtufxvikwfblniwjkxvaezbbrhtghrtrfligsimbaszob";
	int esdjakopqqnbx = 980;
	if (string("pakxpcsengvnmmrruhijgqekjjesxfcumypwqgktxuvwujtufxvikwfblniwjkxvaezbbrhtghrtrfligsimbaszob") != string("pakxpcsengvnmmrruhijgqekjjesxfcumypwqgktxuvwujtufxvikwfblniwjkxvaezbbrhtghrtrfligsimbaszob")) {
		int opnbx;
		for (opnbx = 53; opnbx > 0; opnbx--) {
			continue;
		}
	}
	if (33347 == 33347) {
		int xcbsdqa;
		for (xcbsdqa = 92; xcbsdqa > 0; xcbsdqa--) {
			continue;
		}
	}

}

bool sldprcv::loyeqongvjbnetnuzkzqgxne(int yvxnaxlbvjksec, int zcvukreomh, string rlfhigtsfdg) {
	return true;
}

bool sldprcv::pxaozarbqqjyxovwvcdpggjib() {
	double souyxu = 7057;
	string odzqytdpzlpzlnr = "iusnntuxkxandyfmzsgxzqbkvgqexdgcxanzvf";
	double attzlehs = 14731;
	bool erepaxmnwi = false;
	double tvjhzjqjnetoqat = 4979;
	double mhryiihufma = 16725;
	string rurzbfsrssb = "igjokqdifkhrzllwfngtkiadebcmsucfbfzhgikwiqhhmyydexuo";
	double jcqnbpfzxotd = 74327;
	if (string("igjokqdifkhrzllwfngtkiadebcmsucfbfzhgikwiqhhmyydexuo") == string("igjokqdifkhrzllwfngtkiadebcmsucfbfzhgikwiqhhmyydexuo")) {
		int ezrk;
		for (ezrk = 85; ezrk > 0; ezrk--) {
			continue;
		}
	}
	if (string("igjokqdifkhrzllwfngtkiadebcmsucfbfzhgikwiqhhmyydexuo") != string("igjokqdifkhrzllwfngtkiadebcmsucfbfzhgikwiqhhmyydexuo")) {
		int xiank;
		for (xiank = 96; xiank > 0; xiank--) {
			continue;
		}
	}
	return true;
}

int sldprcv::cubmcxpichzdapcpl(bool hnijcekukgyjt, double yxeodhzifyxuwmf, int wdtilxwpmotiea, double ngraojkz, bool aacqcy, string ihbhid, bool gmwomfurja, string rmmwfwdres, double qfnezjcbuwka) {
	return 18427;
}

double sldprcv::zkxwxhtdcmnnsrvgq(int yttbppzge) {
	return 28035;
}

int sldprcv::rmkgxnlmasqmrb(int fscdjpmelrofifu, int kowkshucltazda, double iajik, int uqptnmiuofxlu) {
	bool ejlnfb = true;
	string nkmktwgcftt = "aiidnkpfyqhfwrvifazluxeyqffjmkjweilnwtepkgaxcrjidorbuxzqqhcxpplanpjbpqnsitxutqstdljxmpcz";
	string ohribkf = "zltomdznkulgtdbqqrztsmtxfejanejifipwjbirgndjmjrfgmprynvorivtxneyixlqepaykbau";
	string anzvaebt = "ivyhibktzrpwjkoltqrbfjgn";
	if (string("zltomdznkulgtdbqqrztsmtxfejanejifipwjbirgndjmjrfgmprynvorivtxneyixlqepaykbau") == string("zltomdznkulgtdbqqrztsmtxfejanejifipwjbirgndjmjrfgmprynvorivtxneyixlqepaykbau")) {
		int dpswc;
		for (dpswc = 15; dpswc > 0; dpswc--) {
			continue;
		}
	}
	if (string("ivyhibktzrpwjkoltqrbfjgn") == string("ivyhibktzrpwjkoltqrbfjgn")) {
		int yqcajglv;
		for (yqcajglv = 53; yqcajglv > 0; yqcajglv--) {
			continue;
		}
	}
	if (string("aiidnkpfyqhfwrvifazluxeyqffjmkjweilnwtepkgaxcrjidorbuxzqqhcxpplanpjbpqnsitxutqstdljxmpcz") == string("aiidnkpfyqhfwrvifazluxeyqffjmkjweilnwtepkgaxcrjidorbuxzqqhcxpplanpjbpqnsitxutqstdljxmpcz")) {
		int avrbah;
		for (avrbah = 56; avrbah > 0; avrbah--) {
			continue;
		}
	}
	if (string("zltomdznkulgtdbqqrztsmtxfejanejifipwjbirgndjmjrfgmprynvorivtxneyixlqepaykbau") != string("zltomdznkulgtdbqqrztsmtxfejanejifipwjbirgndjmjrfgmprynvorivtxneyixlqepaykbau")) {
		int knyer;
		for (knyer = 21; knyer > 0; knyer--) {
			continue;
		}
	}
	return 56236;
}

void sldprcv::zssgfjmmaoluvgoyitorllp(double yccpyrfyxd, int haunng, string vvrtlvzjp, double yptbbruksra, string uohnmxmy, bool jmhakzmjinhqpaf, bool vrznmcvdloir, string uzcjkepdhxqab, double vxjhizqmwnwoj) {
	string banzyqhmcqkgvkj = "ffxsnxqlpftfzthgxprippsvqfrkytfrvonfqpxreuvebwqkdjkwkfjgbnrtoeoqmubdvzxjopdlkchidhsslmjfelgu";
	string rrngdm = "gnoqqwqhofmxarouaizmzixgowyycqwgops";
	string paptpeyut = "byvvjccjkggjm";
	double elekdi = 7385;
	string zhtiioawc = "wybdtqpdimtamarckgwbtathldpzrqefpxejvvvyqfsmipdgdsacjvki";
	if (string("gnoqqwqhofmxarouaizmzixgowyycqwgops") == string("gnoqqwqhofmxarouaizmzixgowyycqwgops")) {
		int gdjezjq;
		for (gdjezjq = 55; gdjezjq > 0; gdjezjq--) {
			continue;
		}
	}
	if (string("byvvjccjkggjm") != string("byvvjccjkggjm")) {
		int cyxr;
		for (cyxr = 62; cyxr > 0; cyxr--) {
			continue;
		}
	}
	if (7385 != 7385) {
		int wwqbnr;
		for (wwqbnr = 94; wwqbnr > 0; wwqbnr--) {
			continue;
		}
	}
	if (7385 != 7385) {
		int ihtddf;
		for (ihtddf = 49; ihtddf > 0; ihtddf--) {
			continue;
		}
	}

}

string sldprcv::asxaapopqjfhaxfirtq(bool rgrdfncmkr, int gafizlys, int gyzcx, bool krxkryzjqwrwqj) {
	string wtthtcsiphf = "yltsycbqdwgflbzilnkkuehyjmytkdsortgukvlybvjqoorsnwincpxofaskbvzjpepjlwngyufehtqnoi";
	string arsqkcilygypgp = "kxzfhaptcs";
	int yfavsmqzhme = 6066;
	bool crmrpmbyv = true;
	double eeooioesmbjky = 33602;
	int njzoicrjhfcebbd = 2691;
	string ipnrbncyhcxsvc = "yobwevmttgxnzzswmyaippdfxmjnyktpwcpuwxrfbwimnt";
	bool cxlevbgmukbxq = false;
	int gpytqckxhgmgagr = 3154;
	bool vrbnmfdeuuix = false;
	if (true == true) {
		int liuaxetw;
		for (liuaxetw = 48; liuaxetw > 0; liuaxetw--) {
			continue;
		}
	}
	return string("uxw");
}

bool sldprcv::fyjytradraahpmbol(bool eowegdmozznrcdz, int cmkbctosftoxchg, string hztdmmganti, string xqzqqdd, string jhdcufgkktbl, double viyavrfaixxv, bool wvjolikgmyz) {
	bool jxbvashst = false;
	string jiqegwei = "vgtmxdojrzidevdquuukxvvf";
	double qnmkus = 12835;
	bool yugmbntvza = false;
	bool jbjxbgbbbegkgt = false;
	bool eedztcvbyaibho = true;
	if (string("vgtmxdojrzidevdquuukxvvf") != string("vgtmxdojrzidevdquuukxvvf")) {
		int ijnxfzfwd;
		for (ijnxfzfwd = 57; ijnxfzfwd > 0; ijnxfzfwd--) {
			continue;
		}
	}
	return true;
}

double sldprcv::nvefasesbwjvbpoktsexer(int ltoiqyowtagrm, int wzikgekhwb, bool aruqvlqrhp, int asvna, int wgxhyshyv, string dooedrdqwbhgwax) {
	double atmvggnmzaxyqqq = 4864;
	bool xoxoxwyylxosk = false;
	return 82448;
}

double sldprcv::hdscosarjxxjs(double lfzwdrtygcqh, bool mldxzegqwh) {
	int hzwpqhebrwrxmg = 221;
	string yflivbbpnl = "hwzlnywmfpbxpzucbdvnvhtbnufcjjvlucvrksbsaoil";
	int ugfdpad = 1636;
	bool offzlba = false;
	int diwwrac = 4832;
	string chajcspubw = "ltwavtcpsnsmexmxfoorpqcusoitgwsyyyowsisqxgagkwnfdcbbpnbdzjkrjlxgpexmnij";
	int iezqdmwo = 1189;
	bool raxqdutudewmeg = false;
	bool ardasruhl = false;
	if (4832 != 4832) {
		int abwtg;
		for (abwtg = 63; abwtg > 0; abwtg--) {
			continue;
		}
	}
	if (string("hwzlnywmfpbxpzucbdvnvhtbnufcjjvlucvrksbsaoil") != string("hwzlnywmfpbxpzucbdvnvhtbnufcjjvlucvrksbsaoil")) {
		int pnkpciqy;
		for (pnkpciqy = 76; pnkpciqy > 0; pnkpciqy--) {
			continue;
		}
	}
	if (1636 != 1636) {
		int wawhsjo;
		for (wawhsjo = 58; wawhsjo > 0; wawhsjo--) {
			continue;
		}
	}
	if (4832 != 4832) {
		int ozzp;
		for (ozzp = 34; ozzp > 0; ozzp--) {
			continue;
		}
	}
	if (false != false) {
		int xgdtzu;
		for (xgdtzu = 97; xgdtzu > 0; xgdtzu--) {
			continue;
		}
	}
	return 64268;
}

double sldprcv::wfiqbnbawnwphuajiq() {
	string kapvceqzyntbi = "otmmadeqlqmubskqiwocjnnqheiwcbhnyljcdvqdk";
	if (string("otmmadeqlqmubskqiwocjnnqheiwcbhnyljcdvqdk") == string("otmmadeqlqmubskqiwocjnnqheiwcbhnyljcdvqdk")) {
		int jvaaylk;
		for (jvaaylk = 88; jvaaylk > 0; jvaaylk--) {
			continue;
		}
	}
	if (string("otmmadeqlqmubskqiwocjnnqheiwcbhnyljcdvqdk") == string("otmmadeqlqmubskqiwocjnnqheiwcbhnyljcdvqdk")) {
		int twqyzlabtr;
		for (twqyzlabtr = 37; twqyzlabtr > 0; twqyzlabtr--) {
			continue;
		}
	}
	return 8351;
}

sldprcv::sldprcv() {
	this->zssgfjmmaoluvgoyitorllp(53788, 412, string("mxotwoqdpqolpfwtxjcjss"), 6573, string("scdtupjzznrbxzjujfowjiwvbsdygvutnsyubgdcqwkepmbbddnldqqhjnddyutgqolxyykmcetmefkc"), true, false, string("xuazxtsrpckgnozczumfnjqgopszsfgwyuecequxiovhhcwnftaxjbgwvumve"), 47746);
	this->asxaapopqjfhaxfirtq(false, 2905, 245, true);
	this->fyjytradraahpmbol(true, 1309, string("adbblpioeqstymnmcdnohbtqjpexwadgvpvqchfqeldshzmeeorrynmbcwmankzcdjeqgjyurv"), string("esuzqagdwojeqjoqwwolsnvvwomatmxswwgsfyvawvsfzvnshpfqlrczqalglogdvarjsbkrlwcxvtbjnjxjjtrxnujvoqj"), string("iufkfgeneghbhveunbyarkowrhjojwhlslddzrcnhbukuhgptlkifbfyntqdfvdbtchcwkqnni"), 15723, true);
	this->nvefasesbwjvbpoktsexer(2764, 5694, true, 3213, 4123, string("dlmczyjoexpeycgnhziutjqzehblxvzzawafvoknkmocwohcxfaocwyiewwnglh"));
	this->hdscosarjxxjs(6882, true);
	this->wfiqbnbawnwphuajiq();
	this->pzozlwrcjwfyho(3815, false, false, 960);
	this->mseymctxjqalvreifd(false, true, 2925, string("lffyhvrdwtkzrplynrukxotxdfichijpxaqhkancgxcedfxihnpdvwmbqaitpabuuogouskblmyafppgxtcvklgirtdb"), 2241, false, false, 45117);
	this->jqnrhycpiydaenylvctmqkxd(2953, true, string("zscvhtjzocamisbilfypihvzopzlrbqrsikpnnjre"), 15397, 48438);
	this->fjkhwxdjzsbidxj(string("tjvmwsnsqkkjfipenonivtbfkpqmaoptkjnhczpcummigsconlvsoxgmiaaxojqpsagulimkajyre"), true, true, string("uhsugiefgnkarmriklkrjwllexyqtqbqwcsfstjczqqrorraxzshivvriikvruerjssuytneglrpcfbrhk"), true, false);
	this->pqkraccqimcvxeqedrpsm(27300, 112, false, false, false, string("bnayctsxumdtduehfc"));
	this->loyeqongvjbnetnuzkzqgxne(3823, 1702, string("uqdmilabpsihqhffpbfzfbmhirlmxmwdpyrwzhrxqwsxdmbtytuwqq"));
	this->pxaozarbqqjyxovwvcdpggjib();
	this->cubmcxpichzdapcpl(false, 26069, 2018, 15331, false, string("hrnkumrvhofyfmavlqnqgnxthcuanphsvzfznsidvpaimeujeadkotadmfdxkdhzcojanctssdgvqzq"), true, string("ogqymbmpgvecnehfnahmpkhjolbwpkcsfipktfbclkl"), 5265);
	this->zkxwxhtdcmnnsrvgq(5723);
	this->rmkgxnlmasqmrb(4635, 8358, 5747, 1183);
	this->wqiburdxyysknabanp(4915, 5221);
	this->djvzrkhomzaigvfnwockz(true, 90787, string("gehqgjblekvjolshmrtzgmbdkcuikezdiqvasemlqmwdcmmtkbltfozhrsmqpuajmr"), string("utqnsdxpassziwxrfqqvbhldbidmjpyqcquxwpnusyltypevssqkvfzdmyaqqepgprkctvudmqgu"), string("hwpajreacsbmrpacvxpnslfqrphnlfkfmzfqtdzpyhwybujjyomnkdmnxrbnzulycssdsagaw"), false, false, 2789);
	this->ggpaytuyvyfflbksfxe(3045, false, 22902, string("wpvhjkjobrmznlbqdnqwaekklbuzapnvcnsthxsrrxoifcuciuakinoesgyjhjrmqvrnkwvlxdednnmnat"), string("uxfcboovhdzgnjvmhufajqznpuuoadhhmrhtbvdjmclmhtvrhujlutfcsqndfzatincxknlrjaqweykjisp"), string("xpupxbkdjwymzgymazhclmdtefylryfrwmvtugxypwyspwyhsojwqbvwsqoyhzsudawbdbsprswiekjdgerepvsduvdtnjoxc"));
	this->nmmchwljvewgsuscswkzcc(string("icmn"), 95769, false, 5241, 994, string("uwtfccknnzztdzyjvxovzqcwckyzhroatiauxpju"));
	this->wwzxuwqkireoz();
}
                                          


















































































































































































































































































// Junk Code By Troll Face & Thaisen's Gen
void RsiWhDFKQmbuizSXnQRb65652230() {     float MdCngcobQJIUImlGCamB5620833 = -317880196;    float MdCngcobQJIUImlGCamB27913478 = -663313186;    float MdCngcobQJIUImlGCamB39656345 = -840161489;    float MdCngcobQJIUImlGCamB39046028 = 89383749;    float MdCngcobQJIUImlGCamB18822244 = -966753625;    float MdCngcobQJIUImlGCamB94238876 = -47805791;    float MdCngcobQJIUImlGCamB44359614 = -51996685;    float MdCngcobQJIUImlGCamB62373169 = -163699288;    float MdCngcobQJIUImlGCamB83879320 = -692892010;    float MdCngcobQJIUImlGCamB28039024 = -276287983;    float MdCngcobQJIUImlGCamB98456952 = -245042628;    float MdCngcobQJIUImlGCamB57860680 = -286185528;    float MdCngcobQJIUImlGCamB6073377 = -187782784;    float MdCngcobQJIUImlGCamB54807096 = -108437873;    float MdCngcobQJIUImlGCamB71777161 = -112687481;    float MdCngcobQJIUImlGCamB53119724 = -70678;    float MdCngcobQJIUImlGCamB50647198 = -654242640;    float MdCngcobQJIUImlGCamB25630174 = -306135360;    float MdCngcobQJIUImlGCamB86023188 = -455803330;    float MdCngcobQJIUImlGCamB61526230 = 30135966;    float MdCngcobQJIUImlGCamB3572196 = -442483153;    float MdCngcobQJIUImlGCamB63575632 = -298066325;    float MdCngcobQJIUImlGCamB34200980 = -441614715;    float MdCngcobQJIUImlGCamB16232001 = -478838959;    float MdCngcobQJIUImlGCamB75382394 = -318971588;    float MdCngcobQJIUImlGCamB83071590 = -988313605;    float MdCngcobQJIUImlGCamB98968534 = -640392858;    float MdCngcobQJIUImlGCamB65975091 = -17861232;    float MdCngcobQJIUImlGCamB58847926 = 45668404;    float MdCngcobQJIUImlGCamB65403784 = -105367175;    float MdCngcobQJIUImlGCamB5077426 = 17820244;    float MdCngcobQJIUImlGCamB83353873 = -981021356;    float MdCngcobQJIUImlGCamB27101709 = -489587184;    float MdCngcobQJIUImlGCamB8695557 = 78483017;    float MdCngcobQJIUImlGCamB74737010 = -773381956;    float MdCngcobQJIUImlGCamB51788993 = -730742155;    float MdCngcobQJIUImlGCamB37740834 = -11794080;    float MdCngcobQJIUImlGCamB40809637 = -472911355;    float MdCngcobQJIUImlGCamB36047742 = -588333346;    float MdCngcobQJIUImlGCamB43116383 = -93159974;    float MdCngcobQJIUImlGCamB29290248 = -525561699;    float MdCngcobQJIUImlGCamB7284163 = -27683684;    float MdCngcobQJIUImlGCamB90162918 = -570692985;    float MdCngcobQJIUImlGCamB5380707 = -110941279;    float MdCngcobQJIUImlGCamB66645585 = -295347476;    float MdCngcobQJIUImlGCamB92769862 = 25026970;    float MdCngcobQJIUImlGCamB27985713 = -554735828;    float MdCngcobQJIUImlGCamB94600859 = 57043904;    float MdCngcobQJIUImlGCamB19023055 = -784044540;    float MdCngcobQJIUImlGCamB18401288 = -373622866;    float MdCngcobQJIUImlGCamB96400671 = -785794386;    float MdCngcobQJIUImlGCamB75764551 = -552516523;    float MdCngcobQJIUImlGCamB18934143 = -755635472;    float MdCngcobQJIUImlGCamB60154890 = -23525777;    float MdCngcobQJIUImlGCamB62080034 = -990549294;    float MdCngcobQJIUImlGCamB42045201 = 80186129;    float MdCngcobQJIUImlGCamB93712498 = -121698472;    float MdCngcobQJIUImlGCamB23424345 = -261322530;    float MdCngcobQJIUImlGCamB63663634 = -591644664;    float MdCngcobQJIUImlGCamB35750653 = -978440021;    float MdCngcobQJIUImlGCamB95270341 = -407412933;    float MdCngcobQJIUImlGCamB78384523 = 65864547;    float MdCngcobQJIUImlGCamB3525243 = -109367692;    float MdCngcobQJIUImlGCamB18475537 = -487524836;    float MdCngcobQJIUImlGCamB22961599 = -194108228;    float MdCngcobQJIUImlGCamB15103080 = -264021272;    float MdCngcobQJIUImlGCamB30758971 = -796598344;    float MdCngcobQJIUImlGCamB97377820 = -166265802;    float MdCngcobQJIUImlGCamB80070085 = -335055918;    float MdCngcobQJIUImlGCamB19988169 = -381945326;    float MdCngcobQJIUImlGCamB15378890 = -988276598;    float MdCngcobQJIUImlGCamB9837562 = -81331285;    float MdCngcobQJIUImlGCamB89582431 = -717802015;    float MdCngcobQJIUImlGCamB42906806 = -262643357;    float MdCngcobQJIUImlGCamB32235982 = -444302335;    float MdCngcobQJIUImlGCamB96288033 = -314799469;    float MdCngcobQJIUImlGCamB73412713 = -727373341;    float MdCngcobQJIUImlGCamB28820274 = -230673436;    float MdCngcobQJIUImlGCamB49586416 = -83491484;    float MdCngcobQJIUImlGCamB82612531 = -243998558;    float MdCngcobQJIUImlGCamB55085878 = -333577777;    float MdCngcobQJIUImlGCamB4367676 = -597436762;    float MdCngcobQJIUImlGCamB46952036 = -233816693;    float MdCngcobQJIUImlGCamB40446639 = -580708730;    float MdCngcobQJIUImlGCamB69003112 = -319572789;    float MdCngcobQJIUImlGCamB29312875 = -429663233;    float MdCngcobQJIUImlGCamB64419730 = -125385885;    float MdCngcobQJIUImlGCamB66946819 = -366061407;    float MdCngcobQJIUImlGCamB46615522 = 69032310;    float MdCngcobQJIUImlGCamB32691809 = -753568086;    float MdCngcobQJIUImlGCamB58076495 = -509043684;    float MdCngcobQJIUImlGCamB14316490 = -750471551;    float MdCngcobQJIUImlGCamB77146003 = -881266692;    float MdCngcobQJIUImlGCamB297090 = -609893325;    float MdCngcobQJIUImlGCamB47846042 = -685747041;    float MdCngcobQJIUImlGCamB50905725 = -491426246;    float MdCngcobQJIUImlGCamB3758920 = -918315992;    float MdCngcobQJIUImlGCamB71687381 = 16831850;    float MdCngcobQJIUImlGCamB82419108 = -916833051;    float MdCngcobQJIUImlGCamB51542505 = -317880196;     MdCngcobQJIUImlGCamB5620833 = MdCngcobQJIUImlGCamB27913478;     MdCngcobQJIUImlGCamB27913478 = MdCngcobQJIUImlGCamB39656345;     MdCngcobQJIUImlGCamB39656345 = MdCngcobQJIUImlGCamB39046028;     MdCngcobQJIUImlGCamB39046028 = MdCngcobQJIUImlGCamB18822244;     MdCngcobQJIUImlGCamB18822244 = MdCngcobQJIUImlGCamB94238876;     MdCngcobQJIUImlGCamB94238876 = MdCngcobQJIUImlGCamB44359614;     MdCngcobQJIUImlGCamB44359614 = MdCngcobQJIUImlGCamB62373169;     MdCngcobQJIUImlGCamB62373169 = MdCngcobQJIUImlGCamB83879320;     MdCngcobQJIUImlGCamB83879320 = MdCngcobQJIUImlGCamB28039024;     MdCngcobQJIUImlGCamB28039024 = MdCngcobQJIUImlGCamB98456952;     MdCngcobQJIUImlGCamB98456952 = MdCngcobQJIUImlGCamB57860680;     MdCngcobQJIUImlGCamB57860680 = MdCngcobQJIUImlGCamB6073377;     MdCngcobQJIUImlGCamB6073377 = MdCngcobQJIUImlGCamB54807096;     MdCngcobQJIUImlGCamB54807096 = MdCngcobQJIUImlGCamB71777161;     MdCngcobQJIUImlGCamB71777161 = MdCngcobQJIUImlGCamB53119724;     MdCngcobQJIUImlGCamB53119724 = MdCngcobQJIUImlGCamB50647198;     MdCngcobQJIUImlGCamB50647198 = MdCngcobQJIUImlGCamB25630174;     MdCngcobQJIUImlGCamB25630174 = MdCngcobQJIUImlGCamB86023188;     MdCngcobQJIUImlGCamB86023188 = MdCngcobQJIUImlGCamB61526230;     MdCngcobQJIUImlGCamB61526230 = MdCngcobQJIUImlGCamB3572196;     MdCngcobQJIUImlGCamB3572196 = MdCngcobQJIUImlGCamB63575632;     MdCngcobQJIUImlGCamB63575632 = MdCngcobQJIUImlGCamB34200980;     MdCngcobQJIUImlGCamB34200980 = MdCngcobQJIUImlGCamB16232001;     MdCngcobQJIUImlGCamB16232001 = MdCngcobQJIUImlGCamB75382394;     MdCngcobQJIUImlGCamB75382394 = MdCngcobQJIUImlGCamB83071590;     MdCngcobQJIUImlGCamB83071590 = MdCngcobQJIUImlGCamB98968534;     MdCngcobQJIUImlGCamB98968534 = MdCngcobQJIUImlGCamB65975091;     MdCngcobQJIUImlGCamB65975091 = MdCngcobQJIUImlGCamB58847926;     MdCngcobQJIUImlGCamB58847926 = MdCngcobQJIUImlGCamB65403784;     MdCngcobQJIUImlGCamB65403784 = MdCngcobQJIUImlGCamB5077426;     MdCngcobQJIUImlGCamB5077426 = MdCngcobQJIUImlGCamB83353873;     MdCngcobQJIUImlGCamB83353873 = MdCngcobQJIUImlGCamB27101709;     MdCngcobQJIUImlGCamB27101709 = MdCngcobQJIUImlGCamB8695557;     MdCngcobQJIUImlGCamB8695557 = MdCngcobQJIUImlGCamB74737010;     MdCngcobQJIUImlGCamB74737010 = MdCngcobQJIUImlGCamB51788993;     MdCngcobQJIUImlGCamB51788993 = MdCngcobQJIUImlGCamB37740834;     MdCngcobQJIUImlGCamB37740834 = MdCngcobQJIUImlGCamB40809637;     MdCngcobQJIUImlGCamB40809637 = MdCngcobQJIUImlGCamB36047742;     MdCngcobQJIUImlGCamB36047742 = MdCngcobQJIUImlGCamB43116383;     MdCngcobQJIUImlGCamB43116383 = MdCngcobQJIUImlGCamB29290248;     MdCngcobQJIUImlGCamB29290248 = MdCngcobQJIUImlGCamB7284163;     MdCngcobQJIUImlGCamB7284163 = MdCngcobQJIUImlGCamB90162918;     MdCngcobQJIUImlGCamB90162918 = MdCngcobQJIUImlGCamB5380707;     MdCngcobQJIUImlGCamB5380707 = MdCngcobQJIUImlGCamB66645585;     MdCngcobQJIUImlGCamB66645585 = MdCngcobQJIUImlGCamB92769862;     MdCngcobQJIUImlGCamB92769862 = MdCngcobQJIUImlGCamB27985713;     MdCngcobQJIUImlGCamB27985713 = MdCngcobQJIUImlGCamB94600859;     MdCngcobQJIUImlGCamB94600859 = MdCngcobQJIUImlGCamB19023055;     MdCngcobQJIUImlGCamB19023055 = MdCngcobQJIUImlGCamB18401288;     MdCngcobQJIUImlGCamB18401288 = MdCngcobQJIUImlGCamB96400671;     MdCngcobQJIUImlGCamB96400671 = MdCngcobQJIUImlGCamB75764551;     MdCngcobQJIUImlGCamB75764551 = MdCngcobQJIUImlGCamB18934143;     MdCngcobQJIUImlGCamB18934143 = MdCngcobQJIUImlGCamB60154890;     MdCngcobQJIUImlGCamB60154890 = MdCngcobQJIUImlGCamB62080034;     MdCngcobQJIUImlGCamB62080034 = MdCngcobQJIUImlGCamB42045201;     MdCngcobQJIUImlGCamB42045201 = MdCngcobQJIUImlGCamB93712498;     MdCngcobQJIUImlGCamB93712498 = MdCngcobQJIUImlGCamB23424345;     MdCngcobQJIUImlGCamB23424345 = MdCngcobQJIUImlGCamB63663634;     MdCngcobQJIUImlGCamB63663634 = MdCngcobQJIUImlGCamB35750653;     MdCngcobQJIUImlGCamB35750653 = MdCngcobQJIUImlGCamB95270341;     MdCngcobQJIUImlGCamB95270341 = MdCngcobQJIUImlGCamB78384523;     MdCngcobQJIUImlGCamB78384523 = MdCngcobQJIUImlGCamB3525243;     MdCngcobQJIUImlGCamB3525243 = MdCngcobQJIUImlGCamB18475537;     MdCngcobQJIUImlGCamB18475537 = MdCngcobQJIUImlGCamB22961599;     MdCngcobQJIUImlGCamB22961599 = MdCngcobQJIUImlGCamB15103080;     MdCngcobQJIUImlGCamB15103080 = MdCngcobQJIUImlGCamB30758971;     MdCngcobQJIUImlGCamB30758971 = MdCngcobQJIUImlGCamB97377820;     MdCngcobQJIUImlGCamB97377820 = MdCngcobQJIUImlGCamB80070085;     MdCngcobQJIUImlGCamB80070085 = MdCngcobQJIUImlGCamB19988169;     MdCngcobQJIUImlGCamB19988169 = MdCngcobQJIUImlGCamB15378890;     MdCngcobQJIUImlGCamB15378890 = MdCngcobQJIUImlGCamB9837562;     MdCngcobQJIUImlGCamB9837562 = MdCngcobQJIUImlGCamB89582431;     MdCngcobQJIUImlGCamB89582431 = MdCngcobQJIUImlGCamB42906806;     MdCngcobQJIUImlGCamB42906806 = MdCngcobQJIUImlGCamB32235982;     MdCngcobQJIUImlGCamB32235982 = MdCngcobQJIUImlGCamB96288033;     MdCngcobQJIUImlGCamB96288033 = MdCngcobQJIUImlGCamB73412713;     MdCngcobQJIUImlGCamB73412713 = MdCngcobQJIUImlGCamB28820274;     MdCngcobQJIUImlGCamB28820274 = MdCngcobQJIUImlGCamB49586416;     MdCngcobQJIUImlGCamB49586416 = MdCngcobQJIUImlGCamB82612531;     MdCngcobQJIUImlGCamB82612531 = MdCngcobQJIUImlGCamB55085878;     MdCngcobQJIUImlGCamB55085878 = MdCngcobQJIUImlGCamB4367676;     MdCngcobQJIUImlGCamB4367676 = MdCngcobQJIUImlGCamB46952036;     MdCngcobQJIUImlGCamB46952036 = MdCngcobQJIUImlGCamB40446639;     MdCngcobQJIUImlGCamB40446639 = MdCngcobQJIUImlGCamB69003112;     MdCngcobQJIUImlGCamB69003112 = MdCngcobQJIUImlGCamB29312875;     MdCngcobQJIUImlGCamB29312875 = MdCngcobQJIUImlGCamB64419730;     MdCngcobQJIUImlGCamB64419730 = MdCngcobQJIUImlGCamB66946819;     MdCngcobQJIUImlGCamB66946819 = MdCngcobQJIUImlGCamB46615522;     MdCngcobQJIUImlGCamB46615522 = MdCngcobQJIUImlGCamB32691809;     MdCngcobQJIUImlGCamB32691809 = MdCngcobQJIUImlGCamB58076495;     MdCngcobQJIUImlGCamB58076495 = MdCngcobQJIUImlGCamB14316490;     MdCngcobQJIUImlGCamB14316490 = MdCngcobQJIUImlGCamB77146003;     MdCngcobQJIUImlGCamB77146003 = MdCngcobQJIUImlGCamB297090;     MdCngcobQJIUImlGCamB297090 = MdCngcobQJIUImlGCamB47846042;     MdCngcobQJIUImlGCamB47846042 = MdCngcobQJIUImlGCamB50905725;     MdCngcobQJIUImlGCamB50905725 = MdCngcobQJIUImlGCamB3758920;     MdCngcobQJIUImlGCamB3758920 = MdCngcobQJIUImlGCamB71687381;     MdCngcobQJIUImlGCamB71687381 = MdCngcobQJIUImlGCamB82419108;     MdCngcobQJIUImlGCamB82419108 = MdCngcobQJIUImlGCamB51542505;     MdCngcobQJIUImlGCamB51542505 = MdCngcobQJIUImlGCamB5620833;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void EluObFVjTQuLiTQwUgTi72310974() {     float NeDqshMNiaMZmkRLFYnX5614362 = -190404338;    float NeDqshMNiaMZmkRLFYnX66485795 = -705908311;    float NeDqshMNiaMZmkRLFYnX40969557 = -797290102;    float NeDqshMNiaMZmkRLFYnX24125889 = -831157081;    float NeDqshMNiaMZmkRLFYnX27662346 = -335108947;    float NeDqshMNiaMZmkRLFYnX29044309 = -847238603;    float NeDqshMNiaMZmkRLFYnX12906950 = -856533152;    float NeDqshMNiaMZmkRLFYnX31587220 = -967876605;    float NeDqshMNiaMZmkRLFYnX66619485 = -406538080;    float NeDqshMNiaMZmkRLFYnX24678538 = 86905146;    float NeDqshMNiaMZmkRLFYnX18777558 = -204514700;    float NeDqshMNiaMZmkRLFYnX72152357 = -21663890;    float NeDqshMNiaMZmkRLFYnX37926291 = -209441259;    float NeDqshMNiaMZmkRLFYnX8651316 = -453590925;    float NeDqshMNiaMZmkRLFYnX16949271 = -75400333;    float NeDqshMNiaMZmkRLFYnX27684271 = -332866328;    float NeDqshMNiaMZmkRLFYnX6499958 = -473857210;    float NeDqshMNiaMZmkRLFYnX48630706 = -703206982;    float NeDqshMNiaMZmkRLFYnX6451459 = 26214403;    float NeDqshMNiaMZmkRLFYnX33599993 = -581888485;    float NeDqshMNiaMZmkRLFYnX83241619 = -152949526;    float NeDqshMNiaMZmkRLFYnX41871898 = -808058209;    float NeDqshMNiaMZmkRLFYnX71079376 = -900572557;    float NeDqshMNiaMZmkRLFYnX59882109 = -359759482;    float NeDqshMNiaMZmkRLFYnX3825084 = -745665441;    float NeDqshMNiaMZmkRLFYnX99645667 = -896519533;    float NeDqshMNiaMZmkRLFYnX69190116 = -941192571;    float NeDqshMNiaMZmkRLFYnX1378616 = -626828220;    float NeDqshMNiaMZmkRLFYnX71526373 = -600401911;    float NeDqshMNiaMZmkRLFYnX41493654 = -880798548;    float NeDqshMNiaMZmkRLFYnX38490104 = -369272990;    float NeDqshMNiaMZmkRLFYnX68077730 = -272843118;    float NeDqshMNiaMZmkRLFYnX12554132 = -691531400;    float NeDqshMNiaMZmkRLFYnX8978394 = -898198715;    float NeDqshMNiaMZmkRLFYnX57095048 = 67304119;    float NeDqshMNiaMZmkRLFYnX94617182 = -810507901;    float NeDqshMNiaMZmkRLFYnX49682202 = -111447458;    float NeDqshMNiaMZmkRLFYnX10727317 = -468174034;    float NeDqshMNiaMZmkRLFYnX71361338 = -201014762;    float NeDqshMNiaMZmkRLFYnX14989276 = -802591660;    float NeDqshMNiaMZmkRLFYnX17145916 = -613505597;    float NeDqshMNiaMZmkRLFYnX92981823 = -221475766;    float NeDqshMNiaMZmkRLFYnX50446279 = -29823240;    float NeDqshMNiaMZmkRLFYnX64612605 = -767905722;    float NeDqshMNiaMZmkRLFYnX50663637 = -768606877;    float NeDqshMNiaMZmkRLFYnX33197831 = -887017701;    float NeDqshMNiaMZmkRLFYnX55683505 = 63983550;    float NeDqshMNiaMZmkRLFYnX69984440 = -551717452;    float NeDqshMNiaMZmkRLFYnX37681445 = -347808401;    float NeDqshMNiaMZmkRLFYnX38446710 = 39352247;    float NeDqshMNiaMZmkRLFYnX9660041 = -364349393;    float NeDqshMNiaMZmkRLFYnX90311808 = -934662271;    float NeDqshMNiaMZmkRLFYnX94382491 = -647689480;    float NeDqshMNiaMZmkRLFYnX70405161 = -555445629;    float NeDqshMNiaMZmkRLFYnX89069355 = -747448571;    float NeDqshMNiaMZmkRLFYnX63742464 = -382346129;    float NeDqshMNiaMZmkRLFYnX95406418 = -805335755;    float NeDqshMNiaMZmkRLFYnX81087448 = -337530621;    float NeDqshMNiaMZmkRLFYnX20300806 = 14508359;    float NeDqshMNiaMZmkRLFYnX28016678 = -438589414;    float NeDqshMNiaMZmkRLFYnX59854192 = -906046032;    float NeDqshMNiaMZmkRLFYnX11528335 = -129704932;    float NeDqshMNiaMZmkRLFYnX60060846 = -267474694;    float NeDqshMNiaMZmkRLFYnX25125831 = -525739532;    float NeDqshMNiaMZmkRLFYnX86188433 = -543821864;    float NeDqshMNiaMZmkRLFYnX50699828 = -931671583;    float NeDqshMNiaMZmkRLFYnX59598225 = -330132491;    float NeDqshMNiaMZmkRLFYnX28947897 = -311242544;    float NeDqshMNiaMZmkRLFYnX51556268 = -420895044;    float NeDqshMNiaMZmkRLFYnX22332088 = -264892432;    float NeDqshMNiaMZmkRLFYnX78002069 = -121418870;    float NeDqshMNiaMZmkRLFYnX95772641 = 94316824;    float NeDqshMNiaMZmkRLFYnX77269368 = -402192221;    float NeDqshMNiaMZmkRLFYnX91462182 = -171193938;    float NeDqshMNiaMZmkRLFYnX16454078 = -968382888;    float NeDqshMNiaMZmkRLFYnX90259796 = -931473760;    float NeDqshMNiaMZmkRLFYnX91425618 = -678234970;    float NeDqshMNiaMZmkRLFYnX6466772 = -32666835;    float NeDqshMNiaMZmkRLFYnX9218472 = -591152605;    float NeDqshMNiaMZmkRLFYnX70627253 = -858647740;    float NeDqshMNiaMZmkRLFYnX43962163 = -860503083;    float NeDqshMNiaMZmkRLFYnX99205676 = -289475119;    float NeDqshMNiaMZmkRLFYnX63697171 = -179019820;    float NeDqshMNiaMZmkRLFYnX33079663 = -539754158;    float NeDqshMNiaMZmkRLFYnX31833613 = -416449156;    float NeDqshMNiaMZmkRLFYnX48178296 = -434610720;    float NeDqshMNiaMZmkRLFYnX73695239 = -625153638;    float NeDqshMNiaMZmkRLFYnX42148971 = -36085771;    float NeDqshMNiaMZmkRLFYnX19909039 = -50750145;    float NeDqshMNiaMZmkRLFYnX93352584 = -550349752;    float NeDqshMNiaMZmkRLFYnX99210764 = 94827853;    float NeDqshMNiaMZmkRLFYnX68594754 = -773916838;    float NeDqshMNiaMZmkRLFYnX90426511 = -382682394;    float NeDqshMNiaMZmkRLFYnX43344661 = -762425348;    float NeDqshMNiaMZmkRLFYnX55135084 = -896545628;    float NeDqshMNiaMZmkRLFYnX5617582 = -383800666;    float NeDqshMNiaMZmkRLFYnX32920977 = -954001072;    float NeDqshMNiaMZmkRLFYnX25320448 = -504083708;    float NeDqshMNiaMZmkRLFYnX78424171 = -124083858;    float NeDqshMNiaMZmkRLFYnX99963809 = -190404338;     NeDqshMNiaMZmkRLFYnX5614362 = NeDqshMNiaMZmkRLFYnX66485795;     NeDqshMNiaMZmkRLFYnX66485795 = NeDqshMNiaMZmkRLFYnX40969557;     NeDqshMNiaMZmkRLFYnX40969557 = NeDqshMNiaMZmkRLFYnX24125889;     NeDqshMNiaMZmkRLFYnX24125889 = NeDqshMNiaMZmkRLFYnX27662346;     NeDqshMNiaMZmkRLFYnX27662346 = NeDqshMNiaMZmkRLFYnX29044309;     NeDqshMNiaMZmkRLFYnX29044309 = NeDqshMNiaMZmkRLFYnX12906950;     NeDqshMNiaMZmkRLFYnX12906950 = NeDqshMNiaMZmkRLFYnX31587220;     NeDqshMNiaMZmkRLFYnX31587220 = NeDqshMNiaMZmkRLFYnX66619485;     NeDqshMNiaMZmkRLFYnX66619485 = NeDqshMNiaMZmkRLFYnX24678538;     NeDqshMNiaMZmkRLFYnX24678538 = NeDqshMNiaMZmkRLFYnX18777558;     NeDqshMNiaMZmkRLFYnX18777558 = NeDqshMNiaMZmkRLFYnX72152357;     NeDqshMNiaMZmkRLFYnX72152357 = NeDqshMNiaMZmkRLFYnX37926291;     NeDqshMNiaMZmkRLFYnX37926291 = NeDqshMNiaMZmkRLFYnX8651316;     NeDqshMNiaMZmkRLFYnX8651316 = NeDqshMNiaMZmkRLFYnX16949271;     NeDqshMNiaMZmkRLFYnX16949271 = NeDqshMNiaMZmkRLFYnX27684271;     NeDqshMNiaMZmkRLFYnX27684271 = NeDqshMNiaMZmkRLFYnX6499958;     NeDqshMNiaMZmkRLFYnX6499958 = NeDqshMNiaMZmkRLFYnX48630706;     NeDqshMNiaMZmkRLFYnX48630706 = NeDqshMNiaMZmkRLFYnX6451459;     NeDqshMNiaMZmkRLFYnX6451459 = NeDqshMNiaMZmkRLFYnX33599993;     NeDqshMNiaMZmkRLFYnX33599993 = NeDqshMNiaMZmkRLFYnX83241619;     NeDqshMNiaMZmkRLFYnX83241619 = NeDqshMNiaMZmkRLFYnX41871898;     NeDqshMNiaMZmkRLFYnX41871898 = NeDqshMNiaMZmkRLFYnX71079376;     NeDqshMNiaMZmkRLFYnX71079376 = NeDqshMNiaMZmkRLFYnX59882109;     NeDqshMNiaMZmkRLFYnX59882109 = NeDqshMNiaMZmkRLFYnX3825084;     NeDqshMNiaMZmkRLFYnX3825084 = NeDqshMNiaMZmkRLFYnX99645667;     NeDqshMNiaMZmkRLFYnX99645667 = NeDqshMNiaMZmkRLFYnX69190116;     NeDqshMNiaMZmkRLFYnX69190116 = NeDqshMNiaMZmkRLFYnX1378616;     NeDqshMNiaMZmkRLFYnX1378616 = NeDqshMNiaMZmkRLFYnX71526373;     NeDqshMNiaMZmkRLFYnX71526373 = NeDqshMNiaMZmkRLFYnX41493654;     NeDqshMNiaMZmkRLFYnX41493654 = NeDqshMNiaMZmkRLFYnX38490104;     NeDqshMNiaMZmkRLFYnX38490104 = NeDqshMNiaMZmkRLFYnX68077730;     NeDqshMNiaMZmkRLFYnX68077730 = NeDqshMNiaMZmkRLFYnX12554132;     NeDqshMNiaMZmkRLFYnX12554132 = NeDqshMNiaMZmkRLFYnX8978394;     NeDqshMNiaMZmkRLFYnX8978394 = NeDqshMNiaMZmkRLFYnX57095048;     NeDqshMNiaMZmkRLFYnX57095048 = NeDqshMNiaMZmkRLFYnX94617182;     NeDqshMNiaMZmkRLFYnX94617182 = NeDqshMNiaMZmkRLFYnX49682202;     NeDqshMNiaMZmkRLFYnX49682202 = NeDqshMNiaMZmkRLFYnX10727317;     NeDqshMNiaMZmkRLFYnX10727317 = NeDqshMNiaMZmkRLFYnX71361338;     NeDqshMNiaMZmkRLFYnX71361338 = NeDqshMNiaMZmkRLFYnX14989276;     NeDqshMNiaMZmkRLFYnX14989276 = NeDqshMNiaMZmkRLFYnX17145916;     NeDqshMNiaMZmkRLFYnX17145916 = NeDqshMNiaMZmkRLFYnX92981823;     NeDqshMNiaMZmkRLFYnX92981823 = NeDqshMNiaMZmkRLFYnX50446279;     NeDqshMNiaMZmkRLFYnX50446279 = NeDqshMNiaMZmkRLFYnX64612605;     NeDqshMNiaMZmkRLFYnX64612605 = NeDqshMNiaMZmkRLFYnX50663637;     NeDqshMNiaMZmkRLFYnX50663637 = NeDqshMNiaMZmkRLFYnX33197831;     NeDqshMNiaMZmkRLFYnX33197831 = NeDqshMNiaMZmkRLFYnX55683505;     NeDqshMNiaMZmkRLFYnX55683505 = NeDqshMNiaMZmkRLFYnX69984440;     NeDqshMNiaMZmkRLFYnX69984440 = NeDqshMNiaMZmkRLFYnX37681445;     NeDqshMNiaMZmkRLFYnX37681445 = NeDqshMNiaMZmkRLFYnX38446710;     NeDqshMNiaMZmkRLFYnX38446710 = NeDqshMNiaMZmkRLFYnX9660041;     NeDqshMNiaMZmkRLFYnX9660041 = NeDqshMNiaMZmkRLFYnX90311808;     NeDqshMNiaMZmkRLFYnX90311808 = NeDqshMNiaMZmkRLFYnX94382491;     NeDqshMNiaMZmkRLFYnX94382491 = NeDqshMNiaMZmkRLFYnX70405161;     NeDqshMNiaMZmkRLFYnX70405161 = NeDqshMNiaMZmkRLFYnX89069355;     NeDqshMNiaMZmkRLFYnX89069355 = NeDqshMNiaMZmkRLFYnX63742464;     NeDqshMNiaMZmkRLFYnX63742464 = NeDqshMNiaMZmkRLFYnX95406418;     NeDqshMNiaMZmkRLFYnX95406418 = NeDqshMNiaMZmkRLFYnX81087448;     NeDqshMNiaMZmkRLFYnX81087448 = NeDqshMNiaMZmkRLFYnX20300806;     NeDqshMNiaMZmkRLFYnX20300806 = NeDqshMNiaMZmkRLFYnX28016678;     NeDqshMNiaMZmkRLFYnX28016678 = NeDqshMNiaMZmkRLFYnX59854192;     NeDqshMNiaMZmkRLFYnX59854192 = NeDqshMNiaMZmkRLFYnX11528335;     NeDqshMNiaMZmkRLFYnX11528335 = NeDqshMNiaMZmkRLFYnX60060846;     NeDqshMNiaMZmkRLFYnX60060846 = NeDqshMNiaMZmkRLFYnX25125831;     NeDqshMNiaMZmkRLFYnX25125831 = NeDqshMNiaMZmkRLFYnX86188433;     NeDqshMNiaMZmkRLFYnX86188433 = NeDqshMNiaMZmkRLFYnX50699828;     NeDqshMNiaMZmkRLFYnX50699828 = NeDqshMNiaMZmkRLFYnX59598225;     NeDqshMNiaMZmkRLFYnX59598225 = NeDqshMNiaMZmkRLFYnX28947897;     NeDqshMNiaMZmkRLFYnX28947897 = NeDqshMNiaMZmkRLFYnX51556268;     NeDqshMNiaMZmkRLFYnX51556268 = NeDqshMNiaMZmkRLFYnX22332088;     NeDqshMNiaMZmkRLFYnX22332088 = NeDqshMNiaMZmkRLFYnX78002069;     NeDqshMNiaMZmkRLFYnX78002069 = NeDqshMNiaMZmkRLFYnX95772641;     NeDqshMNiaMZmkRLFYnX95772641 = NeDqshMNiaMZmkRLFYnX77269368;     NeDqshMNiaMZmkRLFYnX77269368 = NeDqshMNiaMZmkRLFYnX91462182;     NeDqshMNiaMZmkRLFYnX91462182 = NeDqshMNiaMZmkRLFYnX16454078;     NeDqshMNiaMZmkRLFYnX16454078 = NeDqshMNiaMZmkRLFYnX90259796;     NeDqshMNiaMZmkRLFYnX90259796 = NeDqshMNiaMZmkRLFYnX91425618;     NeDqshMNiaMZmkRLFYnX91425618 = NeDqshMNiaMZmkRLFYnX6466772;     NeDqshMNiaMZmkRLFYnX6466772 = NeDqshMNiaMZmkRLFYnX9218472;     NeDqshMNiaMZmkRLFYnX9218472 = NeDqshMNiaMZmkRLFYnX70627253;     NeDqshMNiaMZmkRLFYnX70627253 = NeDqshMNiaMZmkRLFYnX43962163;     NeDqshMNiaMZmkRLFYnX43962163 = NeDqshMNiaMZmkRLFYnX99205676;     NeDqshMNiaMZmkRLFYnX99205676 = NeDqshMNiaMZmkRLFYnX63697171;     NeDqshMNiaMZmkRLFYnX63697171 = NeDqshMNiaMZmkRLFYnX33079663;     NeDqshMNiaMZmkRLFYnX33079663 = NeDqshMNiaMZmkRLFYnX31833613;     NeDqshMNiaMZmkRLFYnX31833613 = NeDqshMNiaMZmkRLFYnX48178296;     NeDqshMNiaMZmkRLFYnX48178296 = NeDqshMNiaMZmkRLFYnX73695239;     NeDqshMNiaMZmkRLFYnX73695239 = NeDqshMNiaMZmkRLFYnX42148971;     NeDqshMNiaMZmkRLFYnX42148971 = NeDqshMNiaMZmkRLFYnX19909039;     NeDqshMNiaMZmkRLFYnX19909039 = NeDqshMNiaMZmkRLFYnX93352584;     NeDqshMNiaMZmkRLFYnX93352584 = NeDqshMNiaMZmkRLFYnX99210764;     NeDqshMNiaMZmkRLFYnX99210764 = NeDqshMNiaMZmkRLFYnX68594754;     NeDqshMNiaMZmkRLFYnX68594754 = NeDqshMNiaMZmkRLFYnX90426511;     NeDqshMNiaMZmkRLFYnX90426511 = NeDqshMNiaMZmkRLFYnX43344661;     NeDqshMNiaMZmkRLFYnX43344661 = NeDqshMNiaMZmkRLFYnX55135084;     NeDqshMNiaMZmkRLFYnX55135084 = NeDqshMNiaMZmkRLFYnX5617582;     NeDqshMNiaMZmkRLFYnX5617582 = NeDqshMNiaMZmkRLFYnX32920977;     NeDqshMNiaMZmkRLFYnX32920977 = NeDqshMNiaMZmkRLFYnX25320448;     NeDqshMNiaMZmkRLFYnX25320448 = NeDqshMNiaMZmkRLFYnX78424171;     NeDqshMNiaMZmkRLFYnX78424171 = NeDqshMNiaMZmkRLFYnX99963809;     NeDqshMNiaMZmkRLFYnX99963809 = NeDqshMNiaMZmkRLFYnX5614362;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void PtSRvUUakdZCaTtvjKBO26727188() {     float tDvqewRfrtItBnyGwoKy76266028 = -808553468;    float tDvqewRfrtItBnyGwoKy35159312 = -577551611;    float tDvqewRfrtItBnyGwoKy20938476 = -139584562;    float tDvqewRfrtItBnyGwoKy64057175 = 78704529;    float tDvqewRfrtItBnyGwoKy49112667 = -118515294;    float tDvqewRfrtItBnyGwoKy19044071 = -548819754;    float tDvqewRfrtItBnyGwoKy31285846 = -855308862;    float tDvqewRfrtItBnyGwoKy88429600 = -214693695;    float tDvqewRfrtItBnyGwoKy12484539 = -499885408;    float tDvqewRfrtItBnyGwoKy47644999 = -214190205;    float tDvqewRfrtItBnyGwoKy61026719 = -239416629;    float tDvqewRfrtItBnyGwoKy17698928 = -337427683;    float tDvqewRfrtItBnyGwoKy17753678 = -118978317;    float tDvqewRfrtItBnyGwoKy92498349 = -254619135;    float tDvqewRfrtItBnyGwoKy99669290 = -225150336;    float tDvqewRfrtItBnyGwoKy60212911 = 97150085;    float tDvqewRfrtItBnyGwoKy352785 = -383042340;    float tDvqewRfrtItBnyGwoKy68924201 = -692373178;    float tDvqewRfrtItBnyGwoKy88159240 = -680973539;    float tDvqewRfrtItBnyGwoKy92386702 = -24254790;    float tDvqewRfrtItBnyGwoKy38377374 = -61628667;    float tDvqewRfrtItBnyGwoKy3267663 = -39343514;    float tDvqewRfrtItBnyGwoKy73023185 = -368415701;    float tDvqewRfrtItBnyGwoKy93731645 = 47839612;    float tDvqewRfrtItBnyGwoKy67949472 = -421570129;    float tDvqewRfrtItBnyGwoKy30243249 = -994803978;    float tDvqewRfrtItBnyGwoKy77450902 = -236370346;    float tDvqewRfrtItBnyGwoKy9160841 = -31951988;    float tDvqewRfrtItBnyGwoKy3668862 = -894760128;    float tDvqewRfrtItBnyGwoKy80665749 = -339193619;    float tDvqewRfrtItBnyGwoKy45562101 = -117706676;    float tDvqewRfrtItBnyGwoKy12589192 = -601140281;    float tDvqewRfrtItBnyGwoKy95499492 = -638493008;    float tDvqewRfrtItBnyGwoKy74343368 = -461728400;    float tDvqewRfrtItBnyGwoKy35119285 = -879642661;    float tDvqewRfrtItBnyGwoKy54610442 = -889734449;    float tDvqewRfrtItBnyGwoKy93473688 = -300040666;    float tDvqewRfrtItBnyGwoKy63519805 = -252287142;    float tDvqewRfrtItBnyGwoKy49894627 = -156743399;    float tDvqewRfrtItBnyGwoKy10092574 = -320200022;    float tDvqewRfrtItBnyGwoKy77219199 = -997524476;    float tDvqewRfrtItBnyGwoKy21464815 = 9135164;    float tDvqewRfrtItBnyGwoKy99413238 = -16577292;    float tDvqewRfrtItBnyGwoKy71120905 = -839880694;    float tDvqewRfrtItBnyGwoKy46617696 = 22746728;    float tDvqewRfrtItBnyGwoKy39603666 = -645354856;    float tDvqewRfrtItBnyGwoKy81814708 = -52086249;    float tDvqewRfrtItBnyGwoKy35598650 = 96152253;    float tDvqewRfrtItBnyGwoKy43122897 = -333572370;    float tDvqewRfrtItBnyGwoKy46874197 = -243656631;    float tDvqewRfrtItBnyGwoKy77849365 = 89078486;    float tDvqewRfrtItBnyGwoKy32183313 = -903881993;    float tDvqewRfrtItBnyGwoKy49458148 = -575487522;    float tDvqewRfrtItBnyGwoKy60632465 = -116327125;    float tDvqewRfrtItBnyGwoKy88349813 = -858389486;    float tDvqewRfrtItBnyGwoKy72998366 = -669209954;    float tDvqewRfrtItBnyGwoKy62136127 = -109135910;    float tDvqewRfrtItBnyGwoKy27206831 = -87424175;    float tDvqewRfrtItBnyGwoKy96107703 = -499725342;    float tDvqewRfrtItBnyGwoKy18869419 = -123711317;    float tDvqewRfrtItBnyGwoKy41593169 = -212449409;    float tDvqewRfrtItBnyGwoKy22125005 = -723356874;    float tDvqewRfrtItBnyGwoKy84760739 = -319933567;    float tDvqewRfrtItBnyGwoKy31818790 = -60691790;    float tDvqewRfrtItBnyGwoKy2082898 = 3516470;    float tDvqewRfrtItBnyGwoKy48437528 = -638276349;    float tDvqewRfrtItBnyGwoKy22199435 = -698934675;    float tDvqewRfrtItBnyGwoKy43410309 = -657249918;    float tDvqewRfrtItBnyGwoKy57379065 = -374976475;    float tDvqewRfrtItBnyGwoKy45058849 = -335415887;    float tDvqewRfrtItBnyGwoKy66739223 = -602809249;    float tDvqewRfrtItBnyGwoKy36832980 = -30755198;    float tDvqewRfrtItBnyGwoKy19029574 = -435629780;    float tDvqewRfrtItBnyGwoKy78066666 = -260773517;    float tDvqewRfrtItBnyGwoKy15167503 = -26730315;    float tDvqewRfrtItBnyGwoKy16912560 = 29236169;    float tDvqewRfrtItBnyGwoKy3854424 = 77233777;    float tDvqewRfrtItBnyGwoKy1902280 = -528535008;    float tDvqewRfrtItBnyGwoKy47113950 = -974907117;    float tDvqewRfrtItBnyGwoKy28345807 = -776215273;    float tDvqewRfrtItBnyGwoKy48428540 = -842717729;    float tDvqewRfrtItBnyGwoKy41852252 = -232522599;    float tDvqewRfrtItBnyGwoKy66037944 = -698379618;    float tDvqewRfrtItBnyGwoKy56794664 = -551103497;    float tDvqewRfrtItBnyGwoKy2816385 = -328272105;    float tDvqewRfrtItBnyGwoKy13378788 = -213824683;    float tDvqewRfrtItBnyGwoKy63131043 = 74347241;    float tDvqewRfrtItBnyGwoKy34867028 = -422165884;    float tDvqewRfrtItBnyGwoKy85993554 = -603338915;    float tDvqewRfrtItBnyGwoKy62120918 = -110432707;    float tDvqewRfrtItBnyGwoKy92474314 = -680598540;    float tDvqewRfrtItBnyGwoKy66266858 = -112616492;    float tDvqewRfrtItBnyGwoKy67412102 = -752561800;    float tDvqewRfrtItBnyGwoKy31025209 = 66967918;    float tDvqewRfrtItBnyGwoKy68499405 = -7750614;    float tDvqewRfrtItBnyGwoKy55094195 = -174167602;    float tDvqewRfrtItBnyGwoKy36704076 = -670931269;    float tDvqewRfrtItBnyGwoKy67594449 = -955885502;    float tDvqewRfrtItBnyGwoKy69038008 = -743397165;    float tDvqewRfrtItBnyGwoKy98180168 = -808553468;     tDvqewRfrtItBnyGwoKy76266028 = tDvqewRfrtItBnyGwoKy35159312;     tDvqewRfrtItBnyGwoKy35159312 = tDvqewRfrtItBnyGwoKy20938476;     tDvqewRfrtItBnyGwoKy20938476 = tDvqewRfrtItBnyGwoKy64057175;     tDvqewRfrtItBnyGwoKy64057175 = tDvqewRfrtItBnyGwoKy49112667;     tDvqewRfrtItBnyGwoKy49112667 = tDvqewRfrtItBnyGwoKy19044071;     tDvqewRfrtItBnyGwoKy19044071 = tDvqewRfrtItBnyGwoKy31285846;     tDvqewRfrtItBnyGwoKy31285846 = tDvqewRfrtItBnyGwoKy88429600;     tDvqewRfrtItBnyGwoKy88429600 = tDvqewRfrtItBnyGwoKy12484539;     tDvqewRfrtItBnyGwoKy12484539 = tDvqewRfrtItBnyGwoKy47644999;     tDvqewRfrtItBnyGwoKy47644999 = tDvqewRfrtItBnyGwoKy61026719;     tDvqewRfrtItBnyGwoKy61026719 = tDvqewRfrtItBnyGwoKy17698928;     tDvqewRfrtItBnyGwoKy17698928 = tDvqewRfrtItBnyGwoKy17753678;     tDvqewRfrtItBnyGwoKy17753678 = tDvqewRfrtItBnyGwoKy92498349;     tDvqewRfrtItBnyGwoKy92498349 = tDvqewRfrtItBnyGwoKy99669290;     tDvqewRfrtItBnyGwoKy99669290 = tDvqewRfrtItBnyGwoKy60212911;     tDvqewRfrtItBnyGwoKy60212911 = tDvqewRfrtItBnyGwoKy352785;     tDvqewRfrtItBnyGwoKy352785 = tDvqewRfrtItBnyGwoKy68924201;     tDvqewRfrtItBnyGwoKy68924201 = tDvqewRfrtItBnyGwoKy88159240;     tDvqewRfrtItBnyGwoKy88159240 = tDvqewRfrtItBnyGwoKy92386702;     tDvqewRfrtItBnyGwoKy92386702 = tDvqewRfrtItBnyGwoKy38377374;     tDvqewRfrtItBnyGwoKy38377374 = tDvqewRfrtItBnyGwoKy3267663;     tDvqewRfrtItBnyGwoKy3267663 = tDvqewRfrtItBnyGwoKy73023185;     tDvqewRfrtItBnyGwoKy73023185 = tDvqewRfrtItBnyGwoKy93731645;     tDvqewRfrtItBnyGwoKy93731645 = tDvqewRfrtItBnyGwoKy67949472;     tDvqewRfrtItBnyGwoKy67949472 = tDvqewRfrtItBnyGwoKy30243249;     tDvqewRfrtItBnyGwoKy30243249 = tDvqewRfrtItBnyGwoKy77450902;     tDvqewRfrtItBnyGwoKy77450902 = tDvqewRfrtItBnyGwoKy9160841;     tDvqewRfrtItBnyGwoKy9160841 = tDvqewRfrtItBnyGwoKy3668862;     tDvqewRfrtItBnyGwoKy3668862 = tDvqewRfrtItBnyGwoKy80665749;     tDvqewRfrtItBnyGwoKy80665749 = tDvqewRfrtItBnyGwoKy45562101;     tDvqewRfrtItBnyGwoKy45562101 = tDvqewRfrtItBnyGwoKy12589192;     tDvqewRfrtItBnyGwoKy12589192 = tDvqewRfrtItBnyGwoKy95499492;     tDvqewRfrtItBnyGwoKy95499492 = tDvqewRfrtItBnyGwoKy74343368;     tDvqewRfrtItBnyGwoKy74343368 = tDvqewRfrtItBnyGwoKy35119285;     tDvqewRfrtItBnyGwoKy35119285 = tDvqewRfrtItBnyGwoKy54610442;     tDvqewRfrtItBnyGwoKy54610442 = tDvqewRfrtItBnyGwoKy93473688;     tDvqewRfrtItBnyGwoKy93473688 = tDvqewRfrtItBnyGwoKy63519805;     tDvqewRfrtItBnyGwoKy63519805 = tDvqewRfrtItBnyGwoKy49894627;     tDvqewRfrtItBnyGwoKy49894627 = tDvqewRfrtItBnyGwoKy10092574;     tDvqewRfrtItBnyGwoKy10092574 = tDvqewRfrtItBnyGwoKy77219199;     tDvqewRfrtItBnyGwoKy77219199 = tDvqewRfrtItBnyGwoKy21464815;     tDvqewRfrtItBnyGwoKy21464815 = tDvqewRfrtItBnyGwoKy99413238;     tDvqewRfrtItBnyGwoKy99413238 = tDvqewRfrtItBnyGwoKy71120905;     tDvqewRfrtItBnyGwoKy71120905 = tDvqewRfrtItBnyGwoKy46617696;     tDvqewRfrtItBnyGwoKy46617696 = tDvqewRfrtItBnyGwoKy39603666;     tDvqewRfrtItBnyGwoKy39603666 = tDvqewRfrtItBnyGwoKy81814708;     tDvqewRfrtItBnyGwoKy81814708 = tDvqewRfrtItBnyGwoKy35598650;     tDvqewRfrtItBnyGwoKy35598650 = tDvqewRfrtItBnyGwoKy43122897;     tDvqewRfrtItBnyGwoKy43122897 = tDvqewRfrtItBnyGwoKy46874197;     tDvqewRfrtItBnyGwoKy46874197 = tDvqewRfrtItBnyGwoKy77849365;     tDvqewRfrtItBnyGwoKy77849365 = tDvqewRfrtItBnyGwoKy32183313;     tDvqewRfrtItBnyGwoKy32183313 = tDvqewRfrtItBnyGwoKy49458148;     tDvqewRfrtItBnyGwoKy49458148 = tDvqewRfrtItBnyGwoKy60632465;     tDvqewRfrtItBnyGwoKy60632465 = tDvqewRfrtItBnyGwoKy88349813;     tDvqewRfrtItBnyGwoKy88349813 = tDvqewRfrtItBnyGwoKy72998366;     tDvqewRfrtItBnyGwoKy72998366 = tDvqewRfrtItBnyGwoKy62136127;     tDvqewRfrtItBnyGwoKy62136127 = tDvqewRfrtItBnyGwoKy27206831;     tDvqewRfrtItBnyGwoKy27206831 = tDvqewRfrtItBnyGwoKy96107703;     tDvqewRfrtItBnyGwoKy96107703 = tDvqewRfrtItBnyGwoKy18869419;     tDvqewRfrtItBnyGwoKy18869419 = tDvqewRfrtItBnyGwoKy41593169;     tDvqewRfrtItBnyGwoKy41593169 = tDvqewRfrtItBnyGwoKy22125005;     tDvqewRfrtItBnyGwoKy22125005 = tDvqewRfrtItBnyGwoKy84760739;     tDvqewRfrtItBnyGwoKy84760739 = tDvqewRfrtItBnyGwoKy31818790;     tDvqewRfrtItBnyGwoKy31818790 = tDvqewRfrtItBnyGwoKy2082898;     tDvqewRfrtItBnyGwoKy2082898 = tDvqewRfrtItBnyGwoKy48437528;     tDvqewRfrtItBnyGwoKy48437528 = tDvqewRfrtItBnyGwoKy22199435;     tDvqewRfrtItBnyGwoKy22199435 = tDvqewRfrtItBnyGwoKy43410309;     tDvqewRfrtItBnyGwoKy43410309 = tDvqewRfrtItBnyGwoKy57379065;     tDvqewRfrtItBnyGwoKy57379065 = tDvqewRfrtItBnyGwoKy45058849;     tDvqewRfrtItBnyGwoKy45058849 = tDvqewRfrtItBnyGwoKy66739223;     tDvqewRfrtItBnyGwoKy66739223 = tDvqewRfrtItBnyGwoKy36832980;     tDvqewRfrtItBnyGwoKy36832980 = tDvqewRfrtItBnyGwoKy19029574;     tDvqewRfrtItBnyGwoKy19029574 = tDvqewRfrtItBnyGwoKy78066666;     tDvqewRfrtItBnyGwoKy78066666 = tDvqewRfrtItBnyGwoKy15167503;     tDvqewRfrtItBnyGwoKy15167503 = tDvqewRfrtItBnyGwoKy16912560;     tDvqewRfrtItBnyGwoKy16912560 = tDvqewRfrtItBnyGwoKy3854424;     tDvqewRfrtItBnyGwoKy3854424 = tDvqewRfrtItBnyGwoKy1902280;     tDvqewRfrtItBnyGwoKy1902280 = tDvqewRfrtItBnyGwoKy47113950;     tDvqewRfrtItBnyGwoKy47113950 = tDvqewRfrtItBnyGwoKy28345807;     tDvqewRfrtItBnyGwoKy28345807 = tDvqewRfrtItBnyGwoKy48428540;     tDvqewRfrtItBnyGwoKy48428540 = tDvqewRfrtItBnyGwoKy41852252;     tDvqewRfrtItBnyGwoKy41852252 = tDvqewRfrtItBnyGwoKy66037944;     tDvqewRfrtItBnyGwoKy66037944 = tDvqewRfrtItBnyGwoKy56794664;     tDvqewRfrtItBnyGwoKy56794664 = tDvqewRfrtItBnyGwoKy2816385;     tDvqewRfrtItBnyGwoKy2816385 = tDvqewRfrtItBnyGwoKy13378788;     tDvqewRfrtItBnyGwoKy13378788 = tDvqewRfrtItBnyGwoKy63131043;     tDvqewRfrtItBnyGwoKy63131043 = tDvqewRfrtItBnyGwoKy34867028;     tDvqewRfrtItBnyGwoKy34867028 = tDvqewRfrtItBnyGwoKy85993554;     tDvqewRfrtItBnyGwoKy85993554 = tDvqewRfrtItBnyGwoKy62120918;     tDvqewRfrtItBnyGwoKy62120918 = tDvqewRfrtItBnyGwoKy92474314;     tDvqewRfrtItBnyGwoKy92474314 = tDvqewRfrtItBnyGwoKy66266858;     tDvqewRfrtItBnyGwoKy66266858 = tDvqewRfrtItBnyGwoKy67412102;     tDvqewRfrtItBnyGwoKy67412102 = tDvqewRfrtItBnyGwoKy31025209;     tDvqewRfrtItBnyGwoKy31025209 = tDvqewRfrtItBnyGwoKy68499405;     tDvqewRfrtItBnyGwoKy68499405 = tDvqewRfrtItBnyGwoKy55094195;     tDvqewRfrtItBnyGwoKy55094195 = tDvqewRfrtItBnyGwoKy36704076;     tDvqewRfrtItBnyGwoKy36704076 = tDvqewRfrtItBnyGwoKy67594449;     tDvqewRfrtItBnyGwoKy67594449 = tDvqewRfrtItBnyGwoKy69038008;     tDvqewRfrtItBnyGwoKy69038008 = tDvqewRfrtItBnyGwoKy98180168;     tDvqewRfrtItBnyGwoKy98180168 = tDvqewRfrtItBnyGwoKy76266028;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void rkuSvkvitMSGhQRaxfWp33385933() {     float pfOtSocCHxySzjneSRZJ76259557 = -681077609;    float pfOtSocCHxySzjneSRZJ73731628 = -620146736;    float pfOtSocCHxySzjneSRZJ22251688 = -96713176;    float pfOtSocCHxySzjneSRZJ49137036 = -841836301;    float pfOtSocCHxySzjneSRZJ57952769 = -586870616;    float pfOtSocCHxySzjneSRZJ53849503 = -248252566;    float pfOtSocCHxySzjneSRZJ99833181 = -559845329;    float pfOtSocCHxySzjneSRZJ57643651 = 81128988;    float pfOtSocCHxySzjneSRZJ95224703 = -213531478;    float pfOtSocCHxySzjneSRZJ44284512 = -950997076;    float pfOtSocCHxySzjneSRZJ81347324 = -198888701;    float pfOtSocCHxySzjneSRZJ31990605 = -72906046;    float pfOtSocCHxySzjneSRZJ49606592 = -140636792;    float pfOtSocCHxySzjneSRZJ46342570 = -599772187;    float pfOtSocCHxySzjneSRZJ44841399 = -187863188;    float pfOtSocCHxySzjneSRZJ34777459 = -235645566;    float pfOtSocCHxySzjneSRZJ56205544 = -202656910;    float pfOtSocCHxySzjneSRZJ91924733 = 10555200;    float pfOtSocCHxySzjneSRZJ8587510 = -198955806;    float pfOtSocCHxySzjneSRZJ64460465 = -636279241;    float pfOtSocCHxySzjneSRZJ18046799 = -872095040;    float pfOtSocCHxySzjneSRZJ81563928 = -549335397;    float pfOtSocCHxySzjneSRZJ9901582 = -827373543;    float pfOtSocCHxySzjneSRZJ37381754 = -933080910;    float pfOtSocCHxySzjneSRZJ96392161 = -848263982;    float pfOtSocCHxySzjneSRZJ46817325 = -903009906;    float pfOtSocCHxySzjneSRZJ47672484 = -537170059;    float pfOtSocCHxySzjneSRZJ44564366 = -640918976;    float pfOtSocCHxySzjneSRZJ16347308 = -440830443;    float pfOtSocCHxySzjneSRZJ56755619 = -14624993;    float pfOtSocCHxySzjneSRZJ78974779 = -504799910;    float pfOtSocCHxySzjneSRZJ97313048 = -992962042;    float pfOtSocCHxySzjneSRZJ80951916 = -840437224;    float pfOtSocCHxySzjneSRZJ74626205 = -338410132;    float pfOtSocCHxySzjneSRZJ17477323 = -38956585;    float pfOtSocCHxySzjneSRZJ97438631 = -969500196;    float pfOtSocCHxySzjneSRZJ5415056 = -399694044;    float pfOtSocCHxySzjneSRZJ33437485 = -247549821;    float pfOtSocCHxySzjneSRZJ85208224 = -869424815;    float pfOtSocCHxySzjneSRZJ81965466 = 70368291;    float pfOtSocCHxySzjneSRZJ65074867 = 14531626;    float pfOtSocCHxySzjneSRZJ7162476 = -184656918;    float pfOtSocCHxySzjneSRZJ59696599 = -575707546;    float pfOtSocCHxySzjneSRZJ30352804 = -396845137;    float pfOtSocCHxySzjneSRZJ30635748 = -450512673;    float pfOtSocCHxySzjneSRZJ80031633 = -457399527;    float pfOtSocCHxySzjneSRZJ9512501 = -533366872;    float pfOtSocCHxySzjneSRZJ10982231 = -512609104;    float pfOtSocCHxySzjneSRZJ61781286 = -997336231;    float pfOtSocCHxySzjneSRZJ66919620 = -930681518;    float pfOtSocCHxySzjneSRZJ91108735 = -589476521;    float pfOtSocCHxySzjneSRZJ46730571 = -186027741;    float pfOtSocCHxySzjneSRZJ24906497 = -467541530;    float pfOtSocCHxySzjneSRZJ70882736 = -648246977;    float pfOtSocCHxySzjneSRZJ15339134 = -615288763;    float pfOtSocCHxySzjneSRZJ94695629 = -31742213;    float pfOtSocCHxySzjneSRZJ63830047 = -792773193;    float pfOtSocCHxySzjneSRZJ84869934 = -163632266;    float pfOtSocCHxySzjneSRZJ52744875 = -993572319;    float pfOtSocCHxySzjneSRZJ11135444 = -683860710;    float pfOtSocCHxySzjneSRZJ6177020 = -711082507;    float pfOtSocCHxySzjneSRZJ55268815 = -918926353;    float pfOtSocCHxySzjneSRZJ41296343 = -478040570;    float pfOtSocCHxySzjneSRZJ38469084 = -98906486;    float pfOtSocCHxySzjneSRZJ65309732 = -346197166;    float pfOtSocCHxySzjneSRZJ84034275 = -205926660;    float pfOtSocCHxySzjneSRZJ51038689 = -232468822;    float pfOtSocCHxySzjneSRZJ74980386 = -802226660;    float pfOtSocCHxySzjneSRZJ28865247 = -460815602;    float pfOtSocCHxySzjneSRZJ47402768 = -218362993;    float pfOtSocCHxySzjneSRZJ29362403 = -835951522;    float pfOtSocCHxySzjneSRZJ22768060 = -955107089;    float pfOtSocCHxySzjneSRZJ6716510 = -120019986;    float pfOtSocCHxySzjneSRZJ26622043 = -169324098;    float pfOtSocCHxySzjneSRZJ99385597 = -550810867;    float pfOtSocCHxySzjneSRZJ10884323 = -587438123;    float pfOtSocCHxySzjneSRZJ21867330 = -973627852;    float pfOtSocCHxySzjneSRZJ79548778 = -330528407;    float pfOtSocCHxySzjneSRZJ6746006 = -382568237;    float pfOtSocCHxySzjneSRZJ16360528 = -290864456;    float pfOtSocCHxySzjneSRZJ37304825 = -269643035;    float pfOtSocCHxySzjneSRZJ36690253 = 75439044;    float pfOtSocCHxySzjneSRZJ82783080 = -643582745;    float pfOtSocCHxySzjneSRZJ49427688 = -510148925;    float pfOtSocCHxySzjneSRZJ65646884 = -425148472;    float pfOtSocCHxySzjneSRZJ32244209 = -218772170;    float pfOtSocCHxySzjneSRZJ72406552 = -425420513;    float pfOtSocCHxySzjneSRZJ10069180 = -92190248;    float pfOtSocCHxySzjneSRZJ59287072 = -723121369;    float pfOtSocCHxySzjneSRZJ22781694 = 92785627;    float pfOtSocCHxySzjneSRZJ33608585 = -76727003;    float pfOtSocCHxySzjneSRZJ20545122 = -136061779;    float pfOtSocCHxySzjneSRZJ80692610 = -253977502;    float pfOtSocCHxySzjneSRZJ74072780 = -85564105;    float pfOtSocCHxySzjneSRZJ75788447 = -218549202;    float pfOtSocCHxySzjneSRZJ9806052 = -66542022;    float pfOtSocCHxySzjneSRZJ65866133 = -706616349;    float pfOtSocCHxySzjneSRZJ21227515 = -376801060;    float pfOtSocCHxySzjneSRZJ65043071 = 49352028;    float pfOtSocCHxySzjneSRZJ46601473 = -681077609;     pfOtSocCHxySzjneSRZJ76259557 = pfOtSocCHxySzjneSRZJ73731628;     pfOtSocCHxySzjneSRZJ73731628 = pfOtSocCHxySzjneSRZJ22251688;     pfOtSocCHxySzjneSRZJ22251688 = pfOtSocCHxySzjneSRZJ49137036;     pfOtSocCHxySzjneSRZJ49137036 = pfOtSocCHxySzjneSRZJ57952769;     pfOtSocCHxySzjneSRZJ57952769 = pfOtSocCHxySzjneSRZJ53849503;     pfOtSocCHxySzjneSRZJ53849503 = pfOtSocCHxySzjneSRZJ99833181;     pfOtSocCHxySzjneSRZJ99833181 = pfOtSocCHxySzjneSRZJ57643651;     pfOtSocCHxySzjneSRZJ57643651 = pfOtSocCHxySzjneSRZJ95224703;     pfOtSocCHxySzjneSRZJ95224703 = pfOtSocCHxySzjneSRZJ44284512;     pfOtSocCHxySzjneSRZJ44284512 = pfOtSocCHxySzjneSRZJ81347324;     pfOtSocCHxySzjneSRZJ81347324 = pfOtSocCHxySzjneSRZJ31990605;     pfOtSocCHxySzjneSRZJ31990605 = pfOtSocCHxySzjneSRZJ49606592;     pfOtSocCHxySzjneSRZJ49606592 = pfOtSocCHxySzjneSRZJ46342570;     pfOtSocCHxySzjneSRZJ46342570 = pfOtSocCHxySzjneSRZJ44841399;     pfOtSocCHxySzjneSRZJ44841399 = pfOtSocCHxySzjneSRZJ34777459;     pfOtSocCHxySzjneSRZJ34777459 = pfOtSocCHxySzjneSRZJ56205544;     pfOtSocCHxySzjneSRZJ56205544 = pfOtSocCHxySzjneSRZJ91924733;     pfOtSocCHxySzjneSRZJ91924733 = pfOtSocCHxySzjneSRZJ8587510;     pfOtSocCHxySzjneSRZJ8587510 = pfOtSocCHxySzjneSRZJ64460465;     pfOtSocCHxySzjneSRZJ64460465 = pfOtSocCHxySzjneSRZJ18046799;     pfOtSocCHxySzjneSRZJ18046799 = pfOtSocCHxySzjneSRZJ81563928;     pfOtSocCHxySzjneSRZJ81563928 = pfOtSocCHxySzjneSRZJ9901582;     pfOtSocCHxySzjneSRZJ9901582 = pfOtSocCHxySzjneSRZJ37381754;     pfOtSocCHxySzjneSRZJ37381754 = pfOtSocCHxySzjneSRZJ96392161;     pfOtSocCHxySzjneSRZJ96392161 = pfOtSocCHxySzjneSRZJ46817325;     pfOtSocCHxySzjneSRZJ46817325 = pfOtSocCHxySzjneSRZJ47672484;     pfOtSocCHxySzjneSRZJ47672484 = pfOtSocCHxySzjneSRZJ44564366;     pfOtSocCHxySzjneSRZJ44564366 = pfOtSocCHxySzjneSRZJ16347308;     pfOtSocCHxySzjneSRZJ16347308 = pfOtSocCHxySzjneSRZJ56755619;     pfOtSocCHxySzjneSRZJ56755619 = pfOtSocCHxySzjneSRZJ78974779;     pfOtSocCHxySzjneSRZJ78974779 = pfOtSocCHxySzjneSRZJ97313048;     pfOtSocCHxySzjneSRZJ97313048 = pfOtSocCHxySzjneSRZJ80951916;     pfOtSocCHxySzjneSRZJ80951916 = pfOtSocCHxySzjneSRZJ74626205;     pfOtSocCHxySzjneSRZJ74626205 = pfOtSocCHxySzjneSRZJ17477323;     pfOtSocCHxySzjneSRZJ17477323 = pfOtSocCHxySzjneSRZJ97438631;     pfOtSocCHxySzjneSRZJ97438631 = pfOtSocCHxySzjneSRZJ5415056;     pfOtSocCHxySzjneSRZJ5415056 = pfOtSocCHxySzjneSRZJ33437485;     pfOtSocCHxySzjneSRZJ33437485 = pfOtSocCHxySzjneSRZJ85208224;     pfOtSocCHxySzjneSRZJ85208224 = pfOtSocCHxySzjneSRZJ81965466;     pfOtSocCHxySzjneSRZJ81965466 = pfOtSocCHxySzjneSRZJ65074867;     pfOtSocCHxySzjneSRZJ65074867 = pfOtSocCHxySzjneSRZJ7162476;     pfOtSocCHxySzjneSRZJ7162476 = pfOtSocCHxySzjneSRZJ59696599;     pfOtSocCHxySzjneSRZJ59696599 = pfOtSocCHxySzjneSRZJ30352804;     pfOtSocCHxySzjneSRZJ30352804 = pfOtSocCHxySzjneSRZJ30635748;     pfOtSocCHxySzjneSRZJ30635748 = pfOtSocCHxySzjneSRZJ80031633;     pfOtSocCHxySzjneSRZJ80031633 = pfOtSocCHxySzjneSRZJ9512501;     pfOtSocCHxySzjneSRZJ9512501 = pfOtSocCHxySzjneSRZJ10982231;     pfOtSocCHxySzjneSRZJ10982231 = pfOtSocCHxySzjneSRZJ61781286;     pfOtSocCHxySzjneSRZJ61781286 = pfOtSocCHxySzjneSRZJ66919620;     pfOtSocCHxySzjneSRZJ66919620 = pfOtSocCHxySzjneSRZJ91108735;     pfOtSocCHxySzjneSRZJ91108735 = pfOtSocCHxySzjneSRZJ46730571;     pfOtSocCHxySzjneSRZJ46730571 = pfOtSocCHxySzjneSRZJ24906497;     pfOtSocCHxySzjneSRZJ24906497 = pfOtSocCHxySzjneSRZJ70882736;     pfOtSocCHxySzjneSRZJ70882736 = pfOtSocCHxySzjneSRZJ15339134;     pfOtSocCHxySzjneSRZJ15339134 = pfOtSocCHxySzjneSRZJ94695629;     pfOtSocCHxySzjneSRZJ94695629 = pfOtSocCHxySzjneSRZJ63830047;     pfOtSocCHxySzjneSRZJ63830047 = pfOtSocCHxySzjneSRZJ84869934;     pfOtSocCHxySzjneSRZJ84869934 = pfOtSocCHxySzjneSRZJ52744875;     pfOtSocCHxySzjneSRZJ52744875 = pfOtSocCHxySzjneSRZJ11135444;     pfOtSocCHxySzjneSRZJ11135444 = pfOtSocCHxySzjneSRZJ6177020;     pfOtSocCHxySzjneSRZJ6177020 = pfOtSocCHxySzjneSRZJ55268815;     pfOtSocCHxySzjneSRZJ55268815 = pfOtSocCHxySzjneSRZJ41296343;     pfOtSocCHxySzjneSRZJ41296343 = pfOtSocCHxySzjneSRZJ38469084;     pfOtSocCHxySzjneSRZJ38469084 = pfOtSocCHxySzjneSRZJ65309732;     pfOtSocCHxySzjneSRZJ65309732 = pfOtSocCHxySzjneSRZJ84034275;     pfOtSocCHxySzjneSRZJ84034275 = pfOtSocCHxySzjneSRZJ51038689;     pfOtSocCHxySzjneSRZJ51038689 = pfOtSocCHxySzjneSRZJ74980386;     pfOtSocCHxySzjneSRZJ74980386 = pfOtSocCHxySzjneSRZJ28865247;     pfOtSocCHxySzjneSRZJ28865247 = pfOtSocCHxySzjneSRZJ47402768;     pfOtSocCHxySzjneSRZJ47402768 = pfOtSocCHxySzjneSRZJ29362403;     pfOtSocCHxySzjneSRZJ29362403 = pfOtSocCHxySzjneSRZJ22768060;     pfOtSocCHxySzjneSRZJ22768060 = pfOtSocCHxySzjneSRZJ6716510;     pfOtSocCHxySzjneSRZJ6716510 = pfOtSocCHxySzjneSRZJ26622043;     pfOtSocCHxySzjneSRZJ26622043 = pfOtSocCHxySzjneSRZJ99385597;     pfOtSocCHxySzjneSRZJ99385597 = pfOtSocCHxySzjneSRZJ10884323;     pfOtSocCHxySzjneSRZJ10884323 = pfOtSocCHxySzjneSRZJ21867330;     pfOtSocCHxySzjneSRZJ21867330 = pfOtSocCHxySzjneSRZJ79548778;     pfOtSocCHxySzjneSRZJ79548778 = pfOtSocCHxySzjneSRZJ6746006;     pfOtSocCHxySzjneSRZJ6746006 = pfOtSocCHxySzjneSRZJ16360528;     pfOtSocCHxySzjneSRZJ16360528 = pfOtSocCHxySzjneSRZJ37304825;     pfOtSocCHxySzjneSRZJ37304825 = pfOtSocCHxySzjneSRZJ36690253;     pfOtSocCHxySzjneSRZJ36690253 = pfOtSocCHxySzjneSRZJ82783080;     pfOtSocCHxySzjneSRZJ82783080 = pfOtSocCHxySzjneSRZJ49427688;     pfOtSocCHxySzjneSRZJ49427688 = pfOtSocCHxySzjneSRZJ65646884;     pfOtSocCHxySzjneSRZJ65646884 = pfOtSocCHxySzjneSRZJ32244209;     pfOtSocCHxySzjneSRZJ32244209 = pfOtSocCHxySzjneSRZJ72406552;     pfOtSocCHxySzjneSRZJ72406552 = pfOtSocCHxySzjneSRZJ10069180;     pfOtSocCHxySzjneSRZJ10069180 = pfOtSocCHxySzjneSRZJ59287072;     pfOtSocCHxySzjneSRZJ59287072 = pfOtSocCHxySzjneSRZJ22781694;     pfOtSocCHxySzjneSRZJ22781694 = pfOtSocCHxySzjneSRZJ33608585;     pfOtSocCHxySzjneSRZJ33608585 = pfOtSocCHxySzjneSRZJ20545122;     pfOtSocCHxySzjneSRZJ20545122 = pfOtSocCHxySzjneSRZJ80692610;     pfOtSocCHxySzjneSRZJ80692610 = pfOtSocCHxySzjneSRZJ74072780;     pfOtSocCHxySzjneSRZJ74072780 = pfOtSocCHxySzjneSRZJ75788447;     pfOtSocCHxySzjneSRZJ75788447 = pfOtSocCHxySzjneSRZJ9806052;     pfOtSocCHxySzjneSRZJ9806052 = pfOtSocCHxySzjneSRZJ65866133;     pfOtSocCHxySzjneSRZJ65866133 = pfOtSocCHxySzjneSRZJ21227515;     pfOtSocCHxySzjneSRZJ21227515 = pfOtSocCHxySzjneSRZJ65043071;     pfOtSocCHxySzjneSRZJ65043071 = pfOtSocCHxySzjneSRZJ46601473;     pfOtSocCHxySzjneSRZJ46601473 = pfOtSocCHxySzjneSRZJ76259557;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void sepMJbgutbGNdmvPLoIh87802145() {     float QOEVYqHEOSjtmOvTnXHD46911224 = -199226740;    float QOEVYqHEOSjtmOvTnXHD42405146 = -491790035;    float QOEVYqHEOSjtmOvTnXHD2220608 = -539007636;    float QOEVYqHEOSjtmOvTnXHD89068322 = 68025310;    float QOEVYqHEOSjtmOvTnXHD79403090 = -370276963;    float QOEVYqHEOSjtmOvTnXHD43849266 = 50166283;    float QOEVYqHEOSjtmOvTnXHD18212077 = -558621039;    float QOEVYqHEOSjtmOvTnXHD14486032 = -265688102;    float QOEVYqHEOSjtmOvTnXHD41089758 = -306878807;    float QOEVYqHEOSjtmOvTnXHD67250973 = -152092426;    float QOEVYqHEOSjtmOvTnXHD23596485 = -233790631;    float QOEVYqHEOSjtmOvTnXHD77537176 = -388669839;    float QOEVYqHEOSjtmOvTnXHD29433979 = -50173850;    float QOEVYqHEOSjtmOvTnXHD30189603 = -400800397;    float QOEVYqHEOSjtmOvTnXHD27561419 = -337613191;    float QOEVYqHEOSjtmOvTnXHD67306099 = -905629152;    float QOEVYqHEOSjtmOvTnXHD50058371 = -111842040;    float QOEVYqHEOSjtmOvTnXHD12218228 = 21389004;    float QOEVYqHEOSjtmOvTnXHD90295291 = -906143748;    float QOEVYqHEOSjtmOvTnXHD23247175 = -78645546;    float QOEVYqHEOSjtmOvTnXHD73182552 = -780774181;    float QOEVYqHEOSjtmOvTnXHD42959693 = -880620703;    float QOEVYqHEOSjtmOvTnXHD11845391 = -295216688;    float QOEVYqHEOSjtmOvTnXHD71231291 = -525481816;    float QOEVYqHEOSjtmOvTnXHD60516550 = -524168670;    float QOEVYqHEOSjtmOvTnXHD77414906 = 98705649;    float QOEVYqHEOSjtmOvTnXHD55933269 = -932347834;    float QOEVYqHEOSjtmOvTnXHD52346591 = -46042744;    float QOEVYqHEOSjtmOvTnXHD48489796 = -735188660;    float QOEVYqHEOSjtmOvTnXHD95927715 = -573020063;    float QOEVYqHEOSjtmOvTnXHD86046776 = -253233596;    float QOEVYqHEOSjtmOvTnXHD41824510 = -221259205;    float QOEVYqHEOSjtmOvTnXHD63897277 = -787398833;    float QOEVYqHEOSjtmOvTnXHD39991181 = 98060184;    float QOEVYqHEOSjtmOvTnXHD95501558 = -985903365;    float QOEVYqHEOSjtmOvTnXHD57431890 = 51273257;    float QOEVYqHEOSjtmOvTnXHD49206542 = -588287252;    float QOEVYqHEOSjtmOvTnXHD86229973 = -31662928;    float QOEVYqHEOSjtmOvTnXHD63741513 = -825153451;    float QOEVYqHEOSjtmOvTnXHD77068765 = -547240071;    float QOEVYqHEOSjtmOvTnXHD25148151 = -369487252;    float QOEVYqHEOSjtmOvTnXHD35645467 = 45954012;    float QOEVYqHEOSjtmOvTnXHD8663559 = -562461598;    float QOEVYqHEOSjtmOvTnXHD36861104 = -468820109;    float QOEVYqHEOSjtmOvTnXHD26589807 = -759159068;    float QOEVYqHEOSjtmOvTnXHD86437468 = -215736682;    float QOEVYqHEOSjtmOvTnXHD35643704 = -649436671;    float QOEVYqHEOSjtmOvTnXHD76596441 = -964739399;    float QOEVYqHEOSjtmOvTnXHD67222738 = -983100201;    float QOEVYqHEOSjtmOvTnXHD75347106 = -113690396;    float QOEVYqHEOSjtmOvTnXHD59298060 = -136048643;    float QOEVYqHEOSjtmOvTnXHD88602075 = -155247463;    float QOEVYqHEOSjtmOvTnXHD79982154 = -395339572;    float QOEVYqHEOSjtmOvTnXHD61110041 = -209128473;    float QOEVYqHEOSjtmOvTnXHD14619593 = -726229678;    float QOEVYqHEOSjtmOvTnXHD3951532 = -318606037;    float QOEVYqHEOSjtmOvTnXHD30559756 = -96573348;    float QOEVYqHEOSjtmOvTnXHD30989316 = 86474180;    float QOEVYqHEOSjtmOvTnXHD28551773 = -407806021;    float QOEVYqHEOSjtmOvTnXHD1988185 = -368982613;    float QOEVYqHEOSjtmOvTnXHD87915996 = -17485884;    float QOEVYqHEOSjtmOvTnXHD65865486 = -412578295;    float QOEVYqHEOSjtmOvTnXHD65996235 = -530499443;    float QOEVYqHEOSjtmOvTnXHD45162042 = -733858744;    float QOEVYqHEOSjtmOvTnXHD81204196 = -898858831;    float QOEVYqHEOSjtmOvTnXHD81771975 = 87468574;    float QOEVYqHEOSjtmOvTnXHD13639900 = -601271006;    float QOEVYqHEOSjtmOvTnXHD89442798 = -48234034;    float QOEVYqHEOSjtmOvTnXHD34688044 = -414897032;    float QOEVYqHEOSjtmOvTnXHD70129528 = -288886448;    float QOEVYqHEOSjtmOvTnXHD18099558 = -217341901;    float QOEVYqHEOSjtmOvTnXHD63828397 = 19820888;    float QOEVYqHEOSjtmOvTnXHD48476715 = -153457545;    float QOEVYqHEOSjtmOvTnXHD13226527 = -258903677;    float QOEVYqHEOSjtmOvTnXHD98099023 = -709158294;    float QOEVYqHEOSjtmOvTnXHD37537086 = -726728194;    float QOEVYqHEOSjtmOvTnXHD34296134 = -218159106;    float QOEVYqHEOSjtmOvTnXHD74984286 = -826396580;    float QOEVYqHEOSjtmOvTnXHD44641484 = -766322749;    float QOEVYqHEOSjtmOvTnXHD74079081 = -208431988;    float QOEVYqHEOSjtmOvTnXHD41771202 = -251857681;    float QOEVYqHEOSjtmOvTnXHD79336828 = -967608436;    float QOEVYqHEOSjtmOvTnXHD85123853 = -62942544;    float QOEVYqHEOSjtmOvTnXHD73142689 = -521498264;    float QOEVYqHEOSjtmOvTnXHD36629656 = -336971421;    float QOEVYqHEOSjtmOvTnXHD97444700 = 2013867;    float QOEVYqHEOSjtmOvTnXHD61842356 = -825919633;    float QOEVYqHEOSjtmOvTnXHD2787237 = -478270360;    float QOEVYqHEOSjtmOvTnXHD25371588 = -175710139;    float QOEVYqHEOSjtmOvTnXHD91550027 = -567297328;    float QOEVYqHEOSjtmOvTnXHD26872135 = -852153396;    float QOEVYqHEOSjtmOvTnXHD18217226 = -574761433;    float QOEVYqHEOSjtmOvTnXHD57678200 = -623856908;    float QOEVYqHEOSjtmOvTnXHD61753328 = -356170839;    float QOEVYqHEOSjtmOvTnXHD89152768 = -429754188;    float QOEVYqHEOSjtmOvTnXHD59282665 = -956908958;    float QOEVYqHEOSjtmOvTnXHD69649231 = -423546545;    float QOEVYqHEOSjtmOvTnXHD63501516 = -828602854;    float QOEVYqHEOSjtmOvTnXHD55656907 = -569961278;    float QOEVYqHEOSjtmOvTnXHD44817831 = -199226740;     QOEVYqHEOSjtmOvTnXHD46911224 = QOEVYqHEOSjtmOvTnXHD42405146;     QOEVYqHEOSjtmOvTnXHD42405146 = QOEVYqHEOSjtmOvTnXHD2220608;     QOEVYqHEOSjtmOvTnXHD2220608 = QOEVYqHEOSjtmOvTnXHD89068322;     QOEVYqHEOSjtmOvTnXHD89068322 = QOEVYqHEOSjtmOvTnXHD79403090;     QOEVYqHEOSjtmOvTnXHD79403090 = QOEVYqHEOSjtmOvTnXHD43849266;     QOEVYqHEOSjtmOvTnXHD43849266 = QOEVYqHEOSjtmOvTnXHD18212077;     QOEVYqHEOSjtmOvTnXHD18212077 = QOEVYqHEOSjtmOvTnXHD14486032;     QOEVYqHEOSjtmOvTnXHD14486032 = QOEVYqHEOSjtmOvTnXHD41089758;     QOEVYqHEOSjtmOvTnXHD41089758 = QOEVYqHEOSjtmOvTnXHD67250973;     QOEVYqHEOSjtmOvTnXHD67250973 = QOEVYqHEOSjtmOvTnXHD23596485;     QOEVYqHEOSjtmOvTnXHD23596485 = QOEVYqHEOSjtmOvTnXHD77537176;     QOEVYqHEOSjtmOvTnXHD77537176 = QOEVYqHEOSjtmOvTnXHD29433979;     QOEVYqHEOSjtmOvTnXHD29433979 = QOEVYqHEOSjtmOvTnXHD30189603;     QOEVYqHEOSjtmOvTnXHD30189603 = QOEVYqHEOSjtmOvTnXHD27561419;     QOEVYqHEOSjtmOvTnXHD27561419 = QOEVYqHEOSjtmOvTnXHD67306099;     QOEVYqHEOSjtmOvTnXHD67306099 = QOEVYqHEOSjtmOvTnXHD50058371;     QOEVYqHEOSjtmOvTnXHD50058371 = QOEVYqHEOSjtmOvTnXHD12218228;     QOEVYqHEOSjtmOvTnXHD12218228 = QOEVYqHEOSjtmOvTnXHD90295291;     QOEVYqHEOSjtmOvTnXHD90295291 = QOEVYqHEOSjtmOvTnXHD23247175;     QOEVYqHEOSjtmOvTnXHD23247175 = QOEVYqHEOSjtmOvTnXHD73182552;     QOEVYqHEOSjtmOvTnXHD73182552 = QOEVYqHEOSjtmOvTnXHD42959693;     QOEVYqHEOSjtmOvTnXHD42959693 = QOEVYqHEOSjtmOvTnXHD11845391;     QOEVYqHEOSjtmOvTnXHD11845391 = QOEVYqHEOSjtmOvTnXHD71231291;     QOEVYqHEOSjtmOvTnXHD71231291 = QOEVYqHEOSjtmOvTnXHD60516550;     QOEVYqHEOSjtmOvTnXHD60516550 = QOEVYqHEOSjtmOvTnXHD77414906;     QOEVYqHEOSjtmOvTnXHD77414906 = QOEVYqHEOSjtmOvTnXHD55933269;     QOEVYqHEOSjtmOvTnXHD55933269 = QOEVYqHEOSjtmOvTnXHD52346591;     QOEVYqHEOSjtmOvTnXHD52346591 = QOEVYqHEOSjtmOvTnXHD48489796;     QOEVYqHEOSjtmOvTnXHD48489796 = QOEVYqHEOSjtmOvTnXHD95927715;     QOEVYqHEOSjtmOvTnXHD95927715 = QOEVYqHEOSjtmOvTnXHD86046776;     QOEVYqHEOSjtmOvTnXHD86046776 = QOEVYqHEOSjtmOvTnXHD41824510;     QOEVYqHEOSjtmOvTnXHD41824510 = QOEVYqHEOSjtmOvTnXHD63897277;     QOEVYqHEOSjtmOvTnXHD63897277 = QOEVYqHEOSjtmOvTnXHD39991181;     QOEVYqHEOSjtmOvTnXHD39991181 = QOEVYqHEOSjtmOvTnXHD95501558;     QOEVYqHEOSjtmOvTnXHD95501558 = QOEVYqHEOSjtmOvTnXHD57431890;     QOEVYqHEOSjtmOvTnXHD57431890 = QOEVYqHEOSjtmOvTnXHD49206542;     QOEVYqHEOSjtmOvTnXHD49206542 = QOEVYqHEOSjtmOvTnXHD86229973;     QOEVYqHEOSjtmOvTnXHD86229973 = QOEVYqHEOSjtmOvTnXHD63741513;     QOEVYqHEOSjtmOvTnXHD63741513 = QOEVYqHEOSjtmOvTnXHD77068765;     QOEVYqHEOSjtmOvTnXHD77068765 = QOEVYqHEOSjtmOvTnXHD25148151;     QOEVYqHEOSjtmOvTnXHD25148151 = QOEVYqHEOSjtmOvTnXHD35645467;     QOEVYqHEOSjtmOvTnXHD35645467 = QOEVYqHEOSjtmOvTnXHD8663559;     QOEVYqHEOSjtmOvTnXHD8663559 = QOEVYqHEOSjtmOvTnXHD36861104;     QOEVYqHEOSjtmOvTnXHD36861104 = QOEVYqHEOSjtmOvTnXHD26589807;     QOEVYqHEOSjtmOvTnXHD26589807 = QOEVYqHEOSjtmOvTnXHD86437468;     QOEVYqHEOSjtmOvTnXHD86437468 = QOEVYqHEOSjtmOvTnXHD35643704;     QOEVYqHEOSjtmOvTnXHD35643704 = QOEVYqHEOSjtmOvTnXHD76596441;     QOEVYqHEOSjtmOvTnXHD76596441 = QOEVYqHEOSjtmOvTnXHD67222738;     QOEVYqHEOSjtmOvTnXHD67222738 = QOEVYqHEOSjtmOvTnXHD75347106;     QOEVYqHEOSjtmOvTnXHD75347106 = QOEVYqHEOSjtmOvTnXHD59298060;     QOEVYqHEOSjtmOvTnXHD59298060 = QOEVYqHEOSjtmOvTnXHD88602075;     QOEVYqHEOSjtmOvTnXHD88602075 = QOEVYqHEOSjtmOvTnXHD79982154;     QOEVYqHEOSjtmOvTnXHD79982154 = QOEVYqHEOSjtmOvTnXHD61110041;     QOEVYqHEOSjtmOvTnXHD61110041 = QOEVYqHEOSjtmOvTnXHD14619593;     QOEVYqHEOSjtmOvTnXHD14619593 = QOEVYqHEOSjtmOvTnXHD3951532;     QOEVYqHEOSjtmOvTnXHD3951532 = QOEVYqHEOSjtmOvTnXHD30559756;     QOEVYqHEOSjtmOvTnXHD30559756 = QOEVYqHEOSjtmOvTnXHD30989316;     QOEVYqHEOSjtmOvTnXHD30989316 = QOEVYqHEOSjtmOvTnXHD28551773;     QOEVYqHEOSjtmOvTnXHD28551773 = QOEVYqHEOSjtmOvTnXHD1988185;     QOEVYqHEOSjtmOvTnXHD1988185 = QOEVYqHEOSjtmOvTnXHD87915996;     QOEVYqHEOSjtmOvTnXHD87915996 = QOEVYqHEOSjtmOvTnXHD65865486;     QOEVYqHEOSjtmOvTnXHD65865486 = QOEVYqHEOSjtmOvTnXHD65996235;     QOEVYqHEOSjtmOvTnXHD65996235 = QOEVYqHEOSjtmOvTnXHD45162042;     QOEVYqHEOSjtmOvTnXHD45162042 = QOEVYqHEOSjtmOvTnXHD81204196;     QOEVYqHEOSjtmOvTnXHD81204196 = QOEVYqHEOSjtmOvTnXHD81771975;     QOEVYqHEOSjtmOvTnXHD81771975 = QOEVYqHEOSjtmOvTnXHD13639900;     QOEVYqHEOSjtmOvTnXHD13639900 = QOEVYqHEOSjtmOvTnXHD89442798;     QOEVYqHEOSjtmOvTnXHD89442798 = QOEVYqHEOSjtmOvTnXHD34688044;     QOEVYqHEOSjtmOvTnXHD34688044 = QOEVYqHEOSjtmOvTnXHD70129528;     QOEVYqHEOSjtmOvTnXHD70129528 = QOEVYqHEOSjtmOvTnXHD18099558;     QOEVYqHEOSjtmOvTnXHD18099558 = QOEVYqHEOSjtmOvTnXHD63828397;     QOEVYqHEOSjtmOvTnXHD63828397 = QOEVYqHEOSjtmOvTnXHD48476715;     QOEVYqHEOSjtmOvTnXHD48476715 = QOEVYqHEOSjtmOvTnXHD13226527;     QOEVYqHEOSjtmOvTnXHD13226527 = QOEVYqHEOSjtmOvTnXHD98099023;     QOEVYqHEOSjtmOvTnXHD98099023 = QOEVYqHEOSjtmOvTnXHD37537086;     QOEVYqHEOSjtmOvTnXHD37537086 = QOEVYqHEOSjtmOvTnXHD34296134;     QOEVYqHEOSjtmOvTnXHD34296134 = QOEVYqHEOSjtmOvTnXHD74984286;     QOEVYqHEOSjtmOvTnXHD74984286 = QOEVYqHEOSjtmOvTnXHD44641484;     QOEVYqHEOSjtmOvTnXHD44641484 = QOEVYqHEOSjtmOvTnXHD74079081;     QOEVYqHEOSjtmOvTnXHD74079081 = QOEVYqHEOSjtmOvTnXHD41771202;     QOEVYqHEOSjtmOvTnXHD41771202 = QOEVYqHEOSjtmOvTnXHD79336828;     QOEVYqHEOSjtmOvTnXHD79336828 = QOEVYqHEOSjtmOvTnXHD85123853;     QOEVYqHEOSjtmOvTnXHD85123853 = QOEVYqHEOSjtmOvTnXHD73142689;     QOEVYqHEOSjtmOvTnXHD73142689 = QOEVYqHEOSjtmOvTnXHD36629656;     QOEVYqHEOSjtmOvTnXHD36629656 = QOEVYqHEOSjtmOvTnXHD97444700;     QOEVYqHEOSjtmOvTnXHD97444700 = QOEVYqHEOSjtmOvTnXHD61842356;     QOEVYqHEOSjtmOvTnXHD61842356 = QOEVYqHEOSjtmOvTnXHD2787237;     QOEVYqHEOSjtmOvTnXHD2787237 = QOEVYqHEOSjtmOvTnXHD25371588;     QOEVYqHEOSjtmOvTnXHD25371588 = QOEVYqHEOSjtmOvTnXHD91550027;     QOEVYqHEOSjtmOvTnXHD91550027 = QOEVYqHEOSjtmOvTnXHD26872135;     QOEVYqHEOSjtmOvTnXHD26872135 = QOEVYqHEOSjtmOvTnXHD18217226;     QOEVYqHEOSjtmOvTnXHD18217226 = QOEVYqHEOSjtmOvTnXHD57678200;     QOEVYqHEOSjtmOvTnXHD57678200 = QOEVYqHEOSjtmOvTnXHD61753328;     QOEVYqHEOSjtmOvTnXHD61753328 = QOEVYqHEOSjtmOvTnXHD89152768;     QOEVYqHEOSjtmOvTnXHD89152768 = QOEVYqHEOSjtmOvTnXHD59282665;     QOEVYqHEOSjtmOvTnXHD59282665 = QOEVYqHEOSjtmOvTnXHD69649231;     QOEVYqHEOSjtmOvTnXHD69649231 = QOEVYqHEOSjtmOvTnXHD63501516;     QOEVYqHEOSjtmOvTnXHD63501516 = QOEVYqHEOSjtmOvTnXHD55656907;     QOEVYqHEOSjtmOvTnXHD55656907 = QOEVYqHEOSjtmOvTnXHD44817831;     QOEVYqHEOSjtmOvTnXHD44817831 = QOEVYqHEOSjtmOvTnXHD46911224;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void UlZHGDDdbLtcxvnUFCTw94460890() {     float IQsAcPiCVPdAldLxsgJT46904753 = -71750881;    float IQsAcPiCVPdAldLxsgJT80977462 = -534385160;    float IQsAcPiCVPdAldLxsgJT3533820 = -496136249;    float IQsAcPiCVPdAldLxsgJT74148183 = -852515520;    float IQsAcPiCVPdAldLxsgJT88243193 = -838632285;    float IQsAcPiCVPdAldLxsgJT78654698 = -749266529;    float IQsAcPiCVPdAldLxsgJT86759413 = -263157506;    float IQsAcPiCVPdAldLxsgJT83700082 = 30134581;    float IQsAcPiCVPdAldLxsgJT23829922 = -20524877;    float IQsAcPiCVPdAldLxsgJT63890486 = -888899297;    float IQsAcPiCVPdAldLxsgJT43917090 = -193262703;    float IQsAcPiCVPdAldLxsgJT91828853 = -124148201;    float IQsAcPiCVPdAldLxsgJT61286892 = -71832324;    float IQsAcPiCVPdAldLxsgJT84033823 = -745953448;    float IQsAcPiCVPdAldLxsgJT72733527 = -300326043;    float IQsAcPiCVPdAldLxsgJT41870646 = -138424803;    float IQsAcPiCVPdAldLxsgJT5911131 = 68543391;    float IQsAcPiCVPdAldLxsgJT35218761 = -375682618;    float IQsAcPiCVPdAldLxsgJT10723562 = -424126015;    float IQsAcPiCVPdAldLxsgJT95320937 = -690669997;    float IQsAcPiCVPdAldLxsgJT52851977 = -491240554;    float IQsAcPiCVPdAldLxsgJT21255959 = -290612586;    float IQsAcPiCVPdAldLxsgJT48723787 = -754174530;    float IQsAcPiCVPdAldLxsgJT14881400 = -406402339;    float IQsAcPiCVPdAldLxsgJT88959239 = -950862523;    float IQsAcPiCVPdAldLxsgJT93988983 = -909500279;    float IQsAcPiCVPdAldLxsgJT26154851 = -133147547;    float IQsAcPiCVPdAldLxsgJT87750116 = -655009732;    float IQsAcPiCVPdAldLxsgJT61168243 = -281258974;    float IQsAcPiCVPdAldLxsgJT72017585 = -248451437;    float IQsAcPiCVPdAldLxsgJT19459455 = -640326830;    float IQsAcPiCVPdAldLxsgJT26548367 = -613080967;    float IQsAcPiCVPdAldLxsgJT49349700 = -989343049;    float IQsAcPiCVPdAldLxsgJT40274018 = -878621549;    float IQsAcPiCVPdAldLxsgJT77859597 = -145217289;    float IQsAcPiCVPdAldLxsgJT260080 = -28492490;    float IQsAcPiCVPdAldLxsgJT61147909 = -687940630;    float IQsAcPiCVPdAldLxsgJT56147653 = -26925607;    float IQsAcPiCVPdAldLxsgJT99055109 = -437834868;    float IQsAcPiCVPdAldLxsgJT48941658 = -156671757;    float IQsAcPiCVPdAldLxsgJT13003819 = -457431151;    float IQsAcPiCVPdAldLxsgJT21343128 = -147838070;    float IQsAcPiCVPdAldLxsgJT68946919 = -21591852;    float IQsAcPiCVPdAldLxsgJT96093002 = -25784552;    float IQsAcPiCVPdAldLxsgJT10607859 = -132418469;    float IQsAcPiCVPdAldLxsgJT26865437 = -27781353;    float IQsAcPiCVPdAldLxsgJT63341496 = -30717293;    float IQsAcPiCVPdAldLxsgJT51980022 = -473500755;    float IQsAcPiCVPdAldLxsgJT85881127 = -546864062;    float IQsAcPiCVPdAldLxsgJT95392529 = -800715283;    float IQsAcPiCVPdAldLxsgJT72557429 = -814603650;    float IQsAcPiCVPdAldLxsgJT3149333 = -537393211;    float IQsAcPiCVPdAldLxsgJT55430502 = -287393581;    float IQsAcPiCVPdAldLxsgJT71360312 = -741048325;    float IQsAcPiCVPdAldLxsgJT41608913 = -483128955;    float IQsAcPiCVPdAldLxsgJT25648795 = -781138296;    float IQsAcPiCVPdAldLxsgJT32253676 = -780210631;    float IQsAcPiCVPdAldLxsgJT88652420 = 10266089;    float IQsAcPiCVPdAldLxsgJT85188944 = -901652998;    float IQsAcPiCVPdAldLxsgJT94254209 = -929132006;    float IQsAcPiCVPdAldLxsgJT52499847 = -516118983;    float IQsAcPiCVPdAldLxsgJT99009296 = -608147774;    float IQsAcPiCVPdAldLxsgJT22531840 = -688606445;    float IQsAcPiCVPdAldLxsgJT51812336 = -772073440;    float IQsAcPiCVPdAldLxsgJT44431032 = -148572468;    float IQsAcPiCVPdAldLxsgJT17368724 = -580181737;    float IQsAcPiCVPdAldLxsgJT42479153 = -134805153;    float IQsAcPiCVPdAldLxsgJT21012875 = -193210776;    float IQsAcPiCVPdAldLxsgJT6174227 = -500736159;    float IQsAcPiCVPdAldLxsgJT72473447 = -171833554;    float IQsAcPiCVPdAldLxsgJT80722736 = -450484173;    float IQsAcPiCVPdAldLxsgJT49763477 = -904531002;    float IQsAcPiCVPdAldLxsgJT36163651 = -937847751;    float IQsAcPiCVPdAldLxsgJT61781904 = -167454258;    float IQsAcPiCVPdAldLxsgJT82317118 = -133238847;    float IQsAcPiCVPdAldLxsgJT31508849 = -243402485;    float IQsAcPiCVPdAldLxsgJT52309040 = -169020735;    float IQsAcPiCVPdAldLxsgJT52630785 = -628389978;    float IQsAcPiCVPdAldLxsgJT4273541 = -173983870;    float IQsAcPiCVPdAldLxsgJT62093803 = -823081171;    float IQsAcPiCVPdAldLxsgJT30647487 = -778782987;    float IQsAcPiCVPdAldLxsgJT74174829 = -659646792;    float IQsAcPiCVPdAldLxsgJT1868989 = -8145671;    float IQsAcPiCVPdAldLxsgJT65775713 = -480543692;    float IQsAcPiCVPdAldLxsgJT99460155 = -433847788;    float IQsAcPiCVPdAldLxsgJT16310123 = -2933620;    float IQsAcPiCVPdAldLxsgJT71117865 = -225687387;    float IQsAcPiCVPdAldLxsgJT77989388 = -148294725;    float IQsAcPiCVPdAldLxsgJT98665104 = -295492594;    float IQsAcPiCVPdAldLxsgJT52210803 = -364078994;    float IQsAcPiCVPdAldLxsgJT68006404 = -248281859;    float IQsAcPiCVPdAldLxsgJT72495489 = -598206720;    float IQsAcPiCVPdAldLxsgJT70958709 = -125272610;    float IQsAcPiCVPdAldLxsgJT4800900 = -508702862;    float IQsAcPiCVPdAldLxsgJT96441810 = -640552775;    float IQsAcPiCVPdAldLxsgJT13994522 = -849283378;    float IQsAcPiCVPdAldLxsgJT98811288 = -459231626;    float IQsAcPiCVPdAldLxsgJT17134583 = -249518412;    float IQsAcPiCVPdAldLxsgJT51661971 = -877212085;    float IQsAcPiCVPdAldLxsgJT93239135 = -71750881;     IQsAcPiCVPdAldLxsgJT46904753 = IQsAcPiCVPdAldLxsgJT80977462;     IQsAcPiCVPdAldLxsgJT80977462 = IQsAcPiCVPdAldLxsgJT3533820;     IQsAcPiCVPdAldLxsgJT3533820 = IQsAcPiCVPdAldLxsgJT74148183;     IQsAcPiCVPdAldLxsgJT74148183 = IQsAcPiCVPdAldLxsgJT88243193;     IQsAcPiCVPdAldLxsgJT88243193 = IQsAcPiCVPdAldLxsgJT78654698;     IQsAcPiCVPdAldLxsgJT78654698 = IQsAcPiCVPdAldLxsgJT86759413;     IQsAcPiCVPdAldLxsgJT86759413 = IQsAcPiCVPdAldLxsgJT83700082;     IQsAcPiCVPdAldLxsgJT83700082 = IQsAcPiCVPdAldLxsgJT23829922;     IQsAcPiCVPdAldLxsgJT23829922 = IQsAcPiCVPdAldLxsgJT63890486;     IQsAcPiCVPdAldLxsgJT63890486 = IQsAcPiCVPdAldLxsgJT43917090;     IQsAcPiCVPdAldLxsgJT43917090 = IQsAcPiCVPdAldLxsgJT91828853;     IQsAcPiCVPdAldLxsgJT91828853 = IQsAcPiCVPdAldLxsgJT61286892;     IQsAcPiCVPdAldLxsgJT61286892 = IQsAcPiCVPdAldLxsgJT84033823;     IQsAcPiCVPdAldLxsgJT84033823 = IQsAcPiCVPdAldLxsgJT72733527;     IQsAcPiCVPdAldLxsgJT72733527 = IQsAcPiCVPdAldLxsgJT41870646;     IQsAcPiCVPdAldLxsgJT41870646 = IQsAcPiCVPdAldLxsgJT5911131;     IQsAcPiCVPdAldLxsgJT5911131 = IQsAcPiCVPdAldLxsgJT35218761;     IQsAcPiCVPdAldLxsgJT35218761 = IQsAcPiCVPdAldLxsgJT10723562;     IQsAcPiCVPdAldLxsgJT10723562 = IQsAcPiCVPdAldLxsgJT95320937;     IQsAcPiCVPdAldLxsgJT95320937 = IQsAcPiCVPdAldLxsgJT52851977;     IQsAcPiCVPdAldLxsgJT52851977 = IQsAcPiCVPdAldLxsgJT21255959;     IQsAcPiCVPdAldLxsgJT21255959 = IQsAcPiCVPdAldLxsgJT48723787;     IQsAcPiCVPdAldLxsgJT48723787 = IQsAcPiCVPdAldLxsgJT14881400;     IQsAcPiCVPdAldLxsgJT14881400 = IQsAcPiCVPdAldLxsgJT88959239;     IQsAcPiCVPdAldLxsgJT88959239 = IQsAcPiCVPdAldLxsgJT93988983;     IQsAcPiCVPdAldLxsgJT93988983 = IQsAcPiCVPdAldLxsgJT26154851;     IQsAcPiCVPdAldLxsgJT26154851 = IQsAcPiCVPdAldLxsgJT87750116;     IQsAcPiCVPdAldLxsgJT87750116 = IQsAcPiCVPdAldLxsgJT61168243;     IQsAcPiCVPdAldLxsgJT61168243 = IQsAcPiCVPdAldLxsgJT72017585;     IQsAcPiCVPdAldLxsgJT72017585 = IQsAcPiCVPdAldLxsgJT19459455;     IQsAcPiCVPdAldLxsgJT19459455 = IQsAcPiCVPdAldLxsgJT26548367;     IQsAcPiCVPdAldLxsgJT26548367 = IQsAcPiCVPdAldLxsgJT49349700;     IQsAcPiCVPdAldLxsgJT49349700 = IQsAcPiCVPdAldLxsgJT40274018;     IQsAcPiCVPdAldLxsgJT40274018 = IQsAcPiCVPdAldLxsgJT77859597;     IQsAcPiCVPdAldLxsgJT77859597 = IQsAcPiCVPdAldLxsgJT260080;     IQsAcPiCVPdAldLxsgJT260080 = IQsAcPiCVPdAldLxsgJT61147909;     IQsAcPiCVPdAldLxsgJT61147909 = IQsAcPiCVPdAldLxsgJT56147653;     IQsAcPiCVPdAldLxsgJT56147653 = IQsAcPiCVPdAldLxsgJT99055109;     IQsAcPiCVPdAldLxsgJT99055109 = IQsAcPiCVPdAldLxsgJT48941658;     IQsAcPiCVPdAldLxsgJT48941658 = IQsAcPiCVPdAldLxsgJT13003819;     IQsAcPiCVPdAldLxsgJT13003819 = IQsAcPiCVPdAldLxsgJT21343128;     IQsAcPiCVPdAldLxsgJT21343128 = IQsAcPiCVPdAldLxsgJT68946919;     IQsAcPiCVPdAldLxsgJT68946919 = IQsAcPiCVPdAldLxsgJT96093002;     IQsAcPiCVPdAldLxsgJT96093002 = IQsAcPiCVPdAldLxsgJT10607859;     IQsAcPiCVPdAldLxsgJT10607859 = IQsAcPiCVPdAldLxsgJT26865437;     IQsAcPiCVPdAldLxsgJT26865437 = IQsAcPiCVPdAldLxsgJT63341496;     IQsAcPiCVPdAldLxsgJT63341496 = IQsAcPiCVPdAldLxsgJT51980022;     IQsAcPiCVPdAldLxsgJT51980022 = IQsAcPiCVPdAldLxsgJT85881127;     IQsAcPiCVPdAldLxsgJT85881127 = IQsAcPiCVPdAldLxsgJT95392529;     IQsAcPiCVPdAldLxsgJT95392529 = IQsAcPiCVPdAldLxsgJT72557429;     IQsAcPiCVPdAldLxsgJT72557429 = IQsAcPiCVPdAldLxsgJT3149333;     IQsAcPiCVPdAldLxsgJT3149333 = IQsAcPiCVPdAldLxsgJT55430502;     IQsAcPiCVPdAldLxsgJT55430502 = IQsAcPiCVPdAldLxsgJT71360312;     IQsAcPiCVPdAldLxsgJT71360312 = IQsAcPiCVPdAldLxsgJT41608913;     IQsAcPiCVPdAldLxsgJT41608913 = IQsAcPiCVPdAldLxsgJT25648795;     IQsAcPiCVPdAldLxsgJT25648795 = IQsAcPiCVPdAldLxsgJT32253676;     IQsAcPiCVPdAldLxsgJT32253676 = IQsAcPiCVPdAldLxsgJT88652420;     IQsAcPiCVPdAldLxsgJT88652420 = IQsAcPiCVPdAldLxsgJT85188944;     IQsAcPiCVPdAldLxsgJT85188944 = IQsAcPiCVPdAldLxsgJT94254209;     IQsAcPiCVPdAldLxsgJT94254209 = IQsAcPiCVPdAldLxsgJT52499847;     IQsAcPiCVPdAldLxsgJT52499847 = IQsAcPiCVPdAldLxsgJT99009296;     IQsAcPiCVPdAldLxsgJT99009296 = IQsAcPiCVPdAldLxsgJT22531840;     IQsAcPiCVPdAldLxsgJT22531840 = IQsAcPiCVPdAldLxsgJT51812336;     IQsAcPiCVPdAldLxsgJT51812336 = IQsAcPiCVPdAldLxsgJT44431032;     IQsAcPiCVPdAldLxsgJT44431032 = IQsAcPiCVPdAldLxsgJT17368724;     IQsAcPiCVPdAldLxsgJT17368724 = IQsAcPiCVPdAldLxsgJT42479153;     IQsAcPiCVPdAldLxsgJT42479153 = IQsAcPiCVPdAldLxsgJT21012875;     IQsAcPiCVPdAldLxsgJT21012875 = IQsAcPiCVPdAldLxsgJT6174227;     IQsAcPiCVPdAldLxsgJT6174227 = IQsAcPiCVPdAldLxsgJT72473447;     IQsAcPiCVPdAldLxsgJT72473447 = IQsAcPiCVPdAldLxsgJT80722736;     IQsAcPiCVPdAldLxsgJT80722736 = IQsAcPiCVPdAldLxsgJT49763477;     IQsAcPiCVPdAldLxsgJT49763477 = IQsAcPiCVPdAldLxsgJT36163651;     IQsAcPiCVPdAldLxsgJT36163651 = IQsAcPiCVPdAldLxsgJT61781904;     IQsAcPiCVPdAldLxsgJT61781904 = IQsAcPiCVPdAldLxsgJT82317118;     IQsAcPiCVPdAldLxsgJT82317118 = IQsAcPiCVPdAldLxsgJT31508849;     IQsAcPiCVPdAldLxsgJT31508849 = IQsAcPiCVPdAldLxsgJT52309040;     IQsAcPiCVPdAldLxsgJT52309040 = IQsAcPiCVPdAldLxsgJT52630785;     IQsAcPiCVPdAldLxsgJT52630785 = IQsAcPiCVPdAldLxsgJT4273541;     IQsAcPiCVPdAldLxsgJT4273541 = IQsAcPiCVPdAldLxsgJT62093803;     IQsAcPiCVPdAldLxsgJT62093803 = IQsAcPiCVPdAldLxsgJT30647487;     IQsAcPiCVPdAldLxsgJT30647487 = IQsAcPiCVPdAldLxsgJT74174829;     IQsAcPiCVPdAldLxsgJT74174829 = IQsAcPiCVPdAldLxsgJT1868989;     IQsAcPiCVPdAldLxsgJT1868989 = IQsAcPiCVPdAldLxsgJT65775713;     IQsAcPiCVPdAldLxsgJT65775713 = IQsAcPiCVPdAldLxsgJT99460155;     IQsAcPiCVPdAldLxsgJT99460155 = IQsAcPiCVPdAldLxsgJT16310123;     IQsAcPiCVPdAldLxsgJT16310123 = IQsAcPiCVPdAldLxsgJT71117865;     IQsAcPiCVPdAldLxsgJT71117865 = IQsAcPiCVPdAldLxsgJT77989388;     IQsAcPiCVPdAldLxsgJT77989388 = IQsAcPiCVPdAldLxsgJT98665104;     IQsAcPiCVPdAldLxsgJT98665104 = IQsAcPiCVPdAldLxsgJT52210803;     IQsAcPiCVPdAldLxsgJT52210803 = IQsAcPiCVPdAldLxsgJT68006404;     IQsAcPiCVPdAldLxsgJT68006404 = IQsAcPiCVPdAldLxsgJT72495489;     IQsAcPiCVPdAldLxsgJT72495489 = IQsAcPiCVPdAldLxsgJT70958709;     IQsAcPiCVPdAldLxsgJT70958709 = IQsAcPiCVPdAldLxsgJT4800900;     IQsAcPiCVPdAldLxsgJT4800900 = IQsAcPiCVPdAldLxsgJT96441810;     IQsAcPiCVPdAldLxsgJT96441810 = IQsAcPiCVPdAldLxsgJT13994522;     IQsAcPiCVPdAldLxsgJT13994522 = IQsAcPiCVPdAldLxsgJT98811288;     IQsAcPiCVPdAldLxsgJT98811288 = IQsAcPiCVPdAldLxsgJT17134583;     IQsAcPiCVPdAldLxsgJT17134583 = IQsAcPiCVPdAldLxsgJT51661971;     IQsAcPiCVPdAldLxsgJT51661971 = IQsAcPiCVPdAldLxsgJT93239135;     IQsAcPiCVPdAldLxsgJT93239135 = IQsAcPiCVPdAldLxsgJT46904753;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void csxrYcjaNMFGgiIpjcQD48877103() {     float jbgGNFdhGcfIUEcnokmE17556420 = -689900012;    float jbgGNFdhGcfIUEcnokmE49650980 = -406028460;    float jbgGNFdhGcfIUEcnokmE83502738 = -938430709;    float jbgGNFdhGcfIUEcnokmE14079471 = 57346091;    float jbgGNFdhGcfIUEcnokmE9693515 = -622038632;    float jbgGNFdhGcfIUEcnokmE68654461 = -450847680;    float jbgGNFdhGcfIUEcnokmE5138309 = -261933215;    float jbgGNFdhGcfIUEcnokmE40542463 = -316682509;    float jbgGNFdhGcfIUEcnokmE69694976 = -113872205;    float jbgGNFdhGcfIUEcnokmE86856947 = -89994648;    float jbgGNFdhGcfIUEcnokmE86166251 = -228164632;    float jbgGNFdhGcfIUEcnokmE37375425 = -439911994;    float jbgGNFdhGcfIUEcnokmE41114280 = 18630617;    float jbgGNFdhGcfIUEcnokmE67880857 = -546981658;    float jbgGNFdhGcfIUEcnokmE55453547 = -450076046;    float jbgGNFdhGcfIUEcnokmE74399286 = -808408390;    float jbgGNFdhGcfIUEcnokmE99763956 = -940641739;    float jbgGNFdhGcfIUEcnokmE55512255 = -364848814;    float jbgGNFdhGcfIUEcnokmE92431343 = -31313957;    float jbgGNFdhGcfIUEcnokmE54107647 = -133036302;    float jbgGNFdhGcfIUEcnokmE7987732 = -399919695;    float jbgGNFdhGcfIUEcnokmE82651723 = -621897892;    float jbgGNFdhGcfIUEcnokmE50667596 = -222017675;    float jbgGNFdhGcfIUEcnokmE48730936 = 1196755;    float jbgGNFdhGcfIUEcnokmE53083628 = -626767211;    float jbgGNFdhGcfIUEcnokmE24586564 = 92215276;    float jbgGNFdhGcfIUEcnokmE34415637 = -528325322;    float jbgGNFdhGcfIUEcnokmE95532341 = -60133500;    float jbgGNFdhGcfIUEcnokmE93310730 = -575617191;    float jbgGNFdhGcfIUEcnokmE11189681 = -806846508;    float jbgGNFdhGcfIUEcnokmE26531452 = -388760516;    float jbgGNFdhGcfIUEcnokmE71059827 = -941378130;    float jbgGNFdhGcfIUEcnokmE32295061 = -936304658;    float jbgGNFdhGcfIUEcnokmE5638993 = -442151233;    float jbgGNFdhGcfIUEcnokmE55883833 = 7835931;    float jbgGNFdhGcfIUEcnokmE60253339 = -107719038;    float jbgGNFdhGcfIUEcnokmE4939396 = -876533838;    float jbgGNFdhGcfIUEcnokmE8940142 = -911038715;    float jbgGNFdhGcfIUEcnokmE77588398 = -393563504;    float jbgGNFdhGcfIUEcnokmE44044956 = -774280120;    float jbgGNFdhGcfIUEcnokmE73077102 = -841450029;    float jbgGNFdhGcfIUEcnokmE49826119 = 82772861;    float jbgGNFdhGcfIUEcnokmE17913879 = -8345904;    float jbgGNFdhGcfIUEcnokmE2601303 = -97759524;    float jbgGNFdhGcfIUEcnokmE6561918 = -441064864;    float jbgGNFdhGcfIUEcnokmE33271272 = -886118507;    float jbgGNFdhGcfIUEcnokmE89472699 = -146787092;    float jbgGNFdhGcfIUEcnokmE17594232 = -925631050;    float jbgGNFdhGcfIUEcnokmE91322580 = -532628031;    float jbgGNFdhGcfIUEcnokmE3820016 = 16275839;    float jbgGNFdhGcfIUEcnokmE40746754 = -361175771;    float jbgGNFdhGcfIUEcnokmE45020837 = -506612933;    float jbgGNFdhGcfIUEcnokmE10506160 = -215191623;    float jbgGNFdhGcfIUEcnokmE61587616 = -301929821;    float jbgGNFdhGcfIUEcnokmE40889372 = -594069870;    float jbgGNFdhGcfIUEcnokmE34904697 = 31997880;    float jbgGNFdhGcfIUEcnokmE98983384 = -84010785;    float jbgGNFdhGcfIUEcnokmE34771802 = -839627465;    float jbgGNFdhGcfIUEcnokmE60995842 = -315886699;    float jbgGNFdhGcfIUEcnokmE85106950 = -614253908;    float jbgGNFdhGcfIUEcnokmE34238824 = -922522359;    float jbgGNFdhGcfIUEcnokmE9605967 = -101799716;    float jbgGNFdhGcfIUEcnokmE47231732 = -741065318;    float jbgGNFdhGcfIUEcnokmE58505295 = -307025698;    float jbgGNFdhGcfIUEcnokmE60325496 = -701234133;    float jbgGNFdhGcfIUEcnokmE15106424 = -286786503;    float jbgGNFdhGcfIUEcnokmE5080364 = -503607337;    float jbgGNFdhGcfIUEcnokmE35475287 = -539218150;    float jbgGNFdhGcfIUEcnokmE11997024 = -454817590;    float jbgGNFdhGcfIUEcnokmE95200208 = -242357009;    float jbgGNFdhGcfIUEcnokmE69459891 = -931874553;    float jbgGNFdhGcfIUEcnokmE90823815 = 70396975;    float jbgGNFdhGcfIUEcnokmE77923856 = -971285310;    float jbgGNFdhGcfIUEcnokmE48386387 = -257033837;    float jbgGNFdhGcfIUEcnokmE81030544 = -291586273;    float jbgGNFdhGcfIUEcnokmE58161613 = -382692556;    float jbgGNFdhGcfIUEcnokmE64737845 = -513551988;    float jbgGNFdhGcfIUEcnokmE48066293 = -24258151;    float jbgGNFdhGcfIUEcnokmE42169019 = -557738382;    float jbgGNFdhGcfIUEcnokmE19812357 = -740648704;    float jbgGNFdhGcfIUEcnokmE35113864 = -760997632;    float jbgGNFdhGcfIUEcnokmE16821405 = -602694272;    float jbgGNFdhGcfIUEcnokmE4209762 = -527505469;    float jbgGNFdhGcfIUEcnokmE89490715 = -491893031;    float jbgGNFdhGcfIUEcnokmE70442927 = -345670737;    float jbgGNFdhGcfIUEcnokmE81510614 = -882147583;    float jbgGNFdhGcfIUEcnokmE60553668 = -626186508;    float jbgGNFdhGcfIUEcnokmE70707444 = -534374837;    float jbgGNFdhGcfIUEcnokmE64749621 = -848081364;    float jbgGNFdhGcfIUEcnokmE20979137 = 75838051;    float jbgGNFdhGcfIUEcnokmE61269954 = 76291747;    float jbgGNFdhGcfIUEcnokmE70167593 = 63093626;    float jbgGNFdhGcfIUEcnokmE47944299 = -495152016;    float jbgGNFdhGcfIUEcnokmE92481447 = -779309597;    float jbgGNFdhGcfIUEcnokmE9806132 = -851757762;    float jbgGNFdhGcfIUEcnokmE63471135 = -639650314;    float jbgGNFdhGcfIUEcnokmE2594387 = -176161822;    float jbgGNFdhGcfIUEcnokmE59408584 = -701320206;    float jbgGNFdhGcfIUEcnokmE42275807 = -396525392;    float jbgGNFdhGcfIUEcnokmE91455494 = -689900012;     jbgGNFdhGcfIUEcnokmE17556420 = jbgGNFdhGcfIUEcnokmE49650980;     jbgGNFdhGcfIUEcnokmE49650980 = jbgGNFdhGcfIUEcnokmE83502738;     jbgGNFdhGcfIUEcnokmE83502738 = jbgGNFdhGcfIUEcnokmE14079471;     jbgGNFdhGcfIUEcnokmE14079471 = jbgGNFdhGcfIUEcnokmE9693515;     jbgGNFdhGcfIUEcnokmE9693515 = jbgGNFdhGcfIUEcnokmE68654461;     jbgGNFdhGcfIUEcnokmE68654461 = jbgGNFdhGcfIUEcnokmE5138309;     jbgGNFdhGcfIUEcnokmE5138309 = jbgGNFdhGcfIUEcnokmE40542463;     jbgGNFdhGcfIUEcnokmE40542463 = jbgGNFdhGcfIUEcnokmE69694976;     jbgGNFdhGcfIUEcnokmE69694976 = jbgGNFdhGcfIUEcnokmE86856947;     jbgGNFdhGcfIUEcnokmE86856947 = jbgGNFdhGcfIUEcnokmE86166251;     jbgGNFdhGcfIUEcnokmE86166251 = jbgGNFdhGcfIUEcnokmE37375425;     jbgGNFdhGcfIUEcnokmE37375425 = jbgGNFdhGcfIUEcnokmE41114280;     jbgGNFdhGcfIUEcnokmE41114280 = jbgGNFdhGcfIUEcnokmE67880857;     jbgGNFdhGcfIUEcnokmE67880857 = jbgGNFdhGcfIUEcnokmE55453547;     jbgGNFdhGcfIUEcnokmE55453547 = jbgGNFdhGcfIUEcnokmE74399286;     jbgGNFdhGcfIUEcnokmE74399286 = jbgGNFdhGcfIUEcnokmE99763956;     jbgGNFdhGcfIUEcnokmE99763956 = jbgGNFdhGcfIUEcnokmE55512255;     jbgGNFdhGcfIUEcnokmE55512255 = jbgGNFdhGcfIUEcnokmE92431343;     jbgGNFdhGcfIUEcnokmE92431343 = jbgGNFdhGcfIUEcnokmE54107647;     jbgGNFdhGcfIUEcnokmE54107647 = jbgGNFdhGcfIUEcnokmE7987732;     jbgGNFdhGcfIUEcnokmE7987732 = jbgGNFdhGcfIUEcnokmE82651723;     jbgGNFdhGcfIUEcnokmE82651723 = jbgGNFdhGcfIUEcnokmE50667596;     jbgGNFdhGcfIUEcnokmE50667596 = jbgGNFdhGcfIUEcnokmE48730936;     jbgGNFdhGcfIUEcnokmE48730936 = jbgGNFdhGcfIUEcnokmE53083628;     jbgGNFdhGcfIUEcnokmE53083628 = jbgGNFdhGcfIUEcnokmE24586564;     jbgGNFdhGcfIUEcnokmE24586564 = jbgGNFdhGcfIUEcnokmE34415637;     jbgGNFdhGcfIUEcnokmE34415637 = jbgGNFdhGcfIUEcnokmE95532341;     jbgGNFdhGcfIUEcnokmE95532341 = jbgGNFdhGcfIUEcnokmE93310730;     jbgGNFdhGcfIUEcnokmE93310730 = jbgGNFdhGcfIUEcnokmE11189681;     jbgGNFdhGcfIUEcnokmE11189681 = jbgGNFdhGcfIUEcnokmE26531452;     jbgGNFdhGcfIUEcnokmE26531452 = jbgGNFdhGcfIUEcnokmE71059827;     jbgGNFdhGcfIUEcnokmE71059827 = jbgGNFdhGcfIUEcnokmE32295061;     jbgGNFdhGcfIUEcnokmE32295061 = jbgGNFdhGcfIUEcnokmE5638993;     jbgGNFdhGcfIUEcnokmE5638993 = jbgGNFdhGcfIUEcnokmE55883833;     jbgGNFdhGcfIUEcnokmE55883833 = jbgGNFdhGcfIUEcnokmE60253339;     jbgGNFdhGcfIUEcnokmE60253339 = jbgGNFdhGcfIUEcnokmE4939396;     jbgGNFdhGcfIUEcnokmE4939396 = jbgGNFdhGcfIUEcnokmE8940142;     jbgGNFdhGcfIUEcnokmE8940142 = jbgGNFdhGcfIUEcnokmE77588398;     jbgGNFdhGcfIUEcnokmE77588398 = jbgGNFdhGcfIUEcnokmE44044956;     jbgGNFdhGcfIUEcnokmE44044956 = jbgGNFdhGcfIUEcnokmE73077102;     jbgGNFdhGcfIUEcnokmE73077102 = jbgGNFdhGcfIUEcnokmE49826119;     jbgGNFdhGcfIUEcnokmE49826119 = jbgGNFdhGcfIUEcnokmE17913879;     jbgGNFdhGcfIUEcnokmE17913879 = jbgGNFdhGcfIUEcnokmE2601303;     jbgGNFdhGcfIUEcnokmE2601303 = jbgGNFdhGcfIUEcnokmE6561918;     jbgGNFdhGcfIUEcnokmE6561918 = jbgGNFdhGcfIUEcnokmE33271272;     jbgGNFdhGcfIUEcnokmE33271272 = jbgGNFdhGcfIUEcnokmE89472699;     jbgGNFdhGcfIUEcnokmE89472699 = jbgGNFdhGcfIUEcnokmE17594232;     jbgGNFdhGcfIUEcnokmE17594232 = jbgGNFdhGcfIUEcnokmE91322580;     jbgGNFdhGcfIUEcnokmE91322580 = jbgGNFdhGcfIUEcnokmE3820016;     jbgGNFdhGcfIUEcnokmE3820016 = jbgGNFdhGcfIUEcnokmE40746754;     jbgGNFdhGcfIUEcnokmE40746754 = jbgGNFdhGcfIUEcnokmE45020837;     jbgGNFdhGcfIUEcnokmE45020837 = jbgGNFdhGcfIUEcnokmE10506160;     jbgGNFdhGcfIUEcnokmE10506160 = jbgGNFdhGcfIUEcnokmE61587616;     jbgGNFdhGcfIUEcnokmE61587616 = jbgGNFdhGcfIUEcnokmE40889372;     jbgGNFdhGcfIUEcnokmE40889372 = jbgGNFdhGcfIUEcnokmE34904697;     jbgGNFdhGcfIUEcnokmE34904697 = jbgGNFdhGcfIUEcnokmE98983384;     jbgGNFdhGcfIUEcnokmE98983384 = jbgGNFdhGcfIUEcnokmE34771802;     jbgGNFdhGcfIUEcnokmE34771802 = jbgGNFdhGcfIUEcnokmE60995842;     jbgGNFdhGcfIUEcnokmE60995842 = jbgGNFdhGcfIUEcnokmE85106950;     jbgGNFdhGcfIUEcnokmE85106950 = jbgGNFdhGcfIUEcnokmE34238824;     jbgGNFdhGcfIUEcnokmE34238824 = jbgGNFdhGcfIUEcnokmE9605967;     jbgGNFdhGcfIUEcnokmE9605967 = jbgGNFdhGcfIUEcnokmE47231732;     jbgGNFdhGcfIUEcnokmE47231732 = jbgGNFdhGcfIUEcnokmE58505295;     jbgGNFdhGcfIUEcnokmE58505295 = jbgGNFdhGcfIUEcnokmE60325496;     jbgGNFdhGcfIUEcnokmE60325496 = jbgGNFdhGcfIUEcnokmE15106424;     jbgGNFdhGcfIUEcnokmE15106424 = jbgGNFdhGcfIUEcnokmE5080364;     jbgGNFdhGcfIUEcnokmE5080364 = jbgGNFdhGcfIUEcnokmE35475287;     jbgGNFdhGcfIUEcnokmE35475287 = jbgGNFdhGcfIUEcnokmE11997024;     jbgGNFdhGcfIUEcnokmE11997024 = jbgGNFdhGcfIUEcnokmE95200208;     jbgGNFdhGcfIUEcnokmE95200208 = jbgGNFdhGcfIUEcnokmE69459891;     jbgGNFdhGcfIUEcnokmE69459891 = jbgGNFdhGcfIUEcnokmE90823815;     jbgGNFdhGcfIUEcnokmE90823815 = jbgGNFdhGcfIUEcnokmE77923856;     jbgGNFdhGcfIUEcnokmE77923856 = jbgGNFdhGcfIUEcnokmE48386387;     jbgGNFdhGcfIUEcnokmE48386387 = jbgGNFdhGcfIUEcnokmE81030544;     jbgGNFdhGcfIUEcnokmE81030544 = jbgGNFdhGcfIUEcnokmE58161613;     jbgGNFdhGcfIUEcnokmE58161613 = jbgGNFdhGcfIUEcnokmE64737845;     jbgGNFdhGcfIUEcnokmE64737845 = jbgGNFdhGcfIUEcnokmE48066293;     jbgGNFdhGcfIUEcnokmE48066293 = jbgGNFdhGcfIUEcnokmE42169019;     jbgGNFdhGcfIUEcnokmE42169019 = jbgGNFdhGcfIUEcnokmE19812357;     jbgGNFdhGcfIUEcnokmE19812357 = jbgGNFdhGcfIUEcnokmE35113864;     jbgGNFdhGcfIUEcnokmE35113864 = jbgGNFdhGcfIUEcnokmE16821405;     jbgGNFdhGcfIUEcnokmE16821405 = jbgGNFdhGcfIUEcnokmE4209762;     jbgGNFdhGcfIUEcnokmE4209762 = jbgGNFdhGcfIUEcnokmE89490715;     jbgGNFdhGcfIUEcnokmE89490715 = jbgGNFdhGcfIUEcnokmE70442927;     jbgGNFdhGcfIUEcnokmE70442927 = jbgGNFdhGcfIUEcnokmE81510614;     jbgGNFdhGcfIUEcnokmE81510614 = jbgGNFdhGcfIUEcnokmE60553668;     jbgGNFdhGcfIUEcnokmE60553668 = jbgGNFdhGcfIUEcnokmE70707444;     jbgGNFdhGcfIUEcnokmE70707444 = jbgGNFdhGcfIUEcnokmE64749621;     jbgGNFdhGcfIUEcnokmE64749621 = jbgGNFdhGcfIUEcnokmE20979137;     jbgGNFdhGcfIUEcnokmE20979137 = jbgGNFdhGcfIUEcnokmE61269954;     jbgGNFdhGcfIUEcnokmE61269954 = jbgGNFdhGcfIUEcnokmE70167593;     jbgGNFdhGcfIUEcnokmE70167593 = jbgGNFdhGcfIUEcnokmE47944299;     jbgGNFdhGcfIUEcnokmE47944299 = jbgGNFdhGcfIUEcnokmE92481447;     jbgGNFdhGcfIUEcnokmE92481447 = jbgGNFdhGcfIUEcnokmE9806132;     jbgGNFdhGcfIUEcnokmE9806132 = jbgGNFdhGcfIUEcnokmE63471135;     jbgGNFdhGcfIUEcnokmE63471135 = jbgGNFdhGcfIUEcnokmE2594387;     jbgGNFdhGcfIUEcnokmE2594387 = jbgGNFdhGcfIUEcnokmE59408584;     jbgGNFdhGcfIUEcnokmE59408584 = jbgGNFdhGcfIUEcnokmE42275807;     jbgGNFdhGcfIUEcnokmE42275807 = jbgGNFdhGcfIUEcnokmE91455494;     jbgGNFdhGcfIUEcnokmE91455494 = jbgGNFdhGcfIUEcnokmE17556420;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void SsWJSefarSZMzpkPquCU55535848() {     float KYeCZGMugtLSVkrbueQA17549949 = -562424153;    float KYeCZGMugtLSVkrbueQA88223296 = -448623585;    float KYeCZGMugtLSVkrbueQA84815950 = -895559323;    float KYeCZGMugtLSVkrbueQA99159330 = -863194740;    float KYeCZGMugtLSVkrbueQA18533617 = 9606047;    float KYeCZGMugtLSVkrbueQA3459894 = -150280492;    float KYeCZGMugtLSVkrbueQA73685644 = 33530318;    float KYeCZGMugtLSVkrbueQA9756513 = -20859826;    float KYeCZGMugtLSVkrbueQA52435140 = -927518275;    float KYeCZGMugtLSVkrbueQA83496461 = -826801519;    float KYeCZGMugtLSVkrbueQA6486857 = -187636704;    float KYeCZGMugtLSVkrbueQA51667102 = -175390357;    float KYeCZGMugtLSVkrbueQA72967193 = -3027857;    float KYeCZGMugtLSVkrbueQA21725077 = -892134710;    float KYeCZGMugtLSVkrbueQA625656 = -412788898;    float KYeCZGMugtLSVkrbueQA48963834 = -41204040;    float KYeCZGMugtLSVkrbueQA55616716 = -760256309;    float KYeCZGMugtLSVkrbueQA78512788 = -761920436;    float KYeCZGMugtLSVkrbueQA12859613 = -649296223;    float KYeCZGMugtLSVkrbueQA26181410 = -745060753;    float KYeCZGMugtLSVkrbueQA87657155 = -110386068;    float KYeCZGMugtLSVkrbueQA60947989 = -31889775;    float KYeCZGMugtLSVkrbueQA87545992 = -680975517;    float KYeCZGMugtLSVkrbueQA92381044 = -979723768;    float KYeCZGMugtLSVkrbueQA81526317 = 46538936;    float KYeCZGMugtLSVkrbueQA41160641 = -915990653;    float KYeCZGMugtLSVkrbueQA4637219 = -829125035;    float KYeCZGMugtLSVkrbueQA30935867 = -669100488;    float KYeCZGMugtLSVkrbueQA5989178 = -121687506;    float KYeCZGMugtLSVkrbueQA87279551 = -482277881;    float KYeCZGMugtLSVkrbueQA59944130 = -775853750;    float KYeCZGMugtLSVkrbueQA55783685 = -233199891;    float KYeCZGMugtLSVkrbueQA17747484 = -38248874;    float KYeCZGMugtLSVkrbueQA5921830 = -318832966;    float KYeCZGMugtLSVkrbueQA38241871 = -251477994;    float KYeCZGMugtLSVkrbueQA3081529 = -187484784;    float KYeCZGMugtLSVkrbueQA16880763 = -976187216;    float KYeCZGMugtLSVkrbueQA78857821 = -906301394;    float KYeCZGMugtLSVkrbueQA12901995 = -6244920;    float KYeCZGMugtLSVkrbueQA15917849 = -383711806;    float KYeCZGMugtLSVkrbueQA60932770 = -929393928;    float KYeCZGMugtLSVkrbueQA35523780 = -111019221;    float KYeCZGMugtLSVkrbueQA78197239 = -567476158;    float KYeCZGMugtLSVkrbueQA61833201 = -754723967;    float KYeCZGMugtLSVkrbueQA90579970 = -914324266;    float KYeCZGMugtLSVkrbueQA73699240 = -698163178;    float KYeCZGMugtLSVkrbueQA17170493 = -628067715;    float KYeCZGMugtLSVkrbueQA92977812 = -434392406;    float KYeCZGMugtLSVkrbueQA9980970 = -96391893;    float KYeCZGMugtLSVkrbueQA23865439 = -670749048;    float KYeCZGMugtLSVkrbueQA54006124 = 60269222;    float KYeCZGMugtLSVkrbueQA59568095 = -888758681;    float KYeCZGMugtLSVkrbueQA85954507 = -107245631;    float KYeCZGMugtLSVkrbueQA71837887 = -833849672;    float KYeCZGMugtLSVkrbueQA67878692 = -350969147;    float KYeCZGMugtLSVkrbueQA56601959 = -430534379;    float KYeCZGMugtLSVkrbueQA677305 = -767648068;    float KYeCZGMugtLSVkrbueQA92434905 = -915835556;    float KYeCZGMugtLSVkrbueQA17633014 = -809733676;    float KYeCZGMugtLSVkrbueQA77372976 = -74403301;    float KYeCZGMugtLSVkrbueQA98822674 = -321155458;    float KYeCZGMugtLSVkrbueQA42749778 = -297369195;    float KYeCZGMugtLSVkrbueQA3767336 = -899172320;    float KYeCZGMugtLSVkrbueQA65155589 = -345240394;    float KYeCZGMugtLSVkrbueQA23552331 = 49052231;    float KYeCZGMugtLSVkrbueQA50703171 = -954436813;    float KYeCZGMugtLSVkrbueQA33919618 = -37141483;    float KYeCZGMugtLSVkrbueQA67045363 = -684194892;    float KYeCZGMugtLSVkrbueQA83483205 = -540656717;    float KYeCZGMugtLSVkrbueQA97544127 = -125304115;    float KYeCZGMugtLSVkrbueQA32083071 = -65016825;    float KYeCZGMugtLSVkrbueQA76758895 = -853954916;    float KYeCZGMugtLSVkrbueQA65610793 = -655675516;    float KYeCZGMugtLSVkrbueQA96941764 = -165584418;    float KYeCZGMugtLSVkrbueQA65248639 = -815666826;    float KYeCZGMugtLSVkrbueQA52133376 = -999366847;    float KYeCZGMugtLSVkrbueQA82750750 = -464413617;    float KYeCZGMugtLSVkrbueQA25712791 = -926251550;    float KYeCZGMugtLSVkrbueQA1801075 = 34600497;    float KYeCZGMugtLSVkrbueQA7827078 = -255297886;    float KYeCZGMugtLSVkrbueQA23990149 = -187922938;    float KYeCZGMugtLSVkrbueQA11659406 = -294732629;    float KYeCZGMugtLSVkrbueQA20954897 = -472708596;    float KYeCZGMugtLSVkrbueQA82123739 = -450938459;    float KYeCZGMugtLSVkrbueQA33273428 = -442547104;    float KYeCZGMugtLSVkrbueQA376036 = -887095070;    float KYeCZGMugtLSVkrbueQA69829177 = -25954261;    float KYeCZGMugtLSVkrbueQA45909597 = -204399202;    float KYeCZGMugtLSVkrbueQA38043138 = -967863819;    float KYeCZGMugtLSVkrbueQA81639912 = -820943616;    float KYeCZGMugtLSVkrbueQA2404225 = -419836716;    float KYeCZGMugtLSVkrbueQA24445857 = 39648339;    float KYeCZGMugtLSVkrbueQA61224808 = 3432282;    float KYeCZGMugtLSVkrbueQA35529019 = -931841620;    float KYeCZGMugtLSVkrbueQA17095174 = 37443651;    float KYeCZGMugtLSVkrbueQA18182992 = -532024733;    float KYeCZGMugtLSVkrbueQA31756444 = -211846902;    float KYeCZGMugtLSVkrbueQA13041650 = -122235764;    float KYeCZGMugtLSVkrbueQA38280871 = -703776199;    float KYeCZGMugtLSVkrbueQA39876799 = -562424153;     KYeCZGMugtLSVkrbueQA17549949 = KYeCZGMugtLSVkrbueQA88223296;     KYeCZGMugtLSVkrbueQA88223296 = KYeCZGMugtLSVkrbueQA84815950;     KYeCZGMugtLSVkrbueQA84815950 = KYeCZGMugtLSVkrbueQA99159330;     KYeCZGMugtLSVkrbueQA99159330 = KYeCZGMugtLSVkrbueQA18533617;     KYeCZGMugtLSVkrbueQA18533617 = KYeCZGMugtLSVkrbueQA3459894;     KYeCZGMugtLSVkrbueQA3459894 = KYeCZGMugtLSVkrbueQA73685644;     KYeCZGMugtLSVkrbueQA73685644 = KYeCZGMugtLSVkrbueQA9756513;     KYeCZGMugtLSVkrbueQA9756513 = KYeCZGMugtLSVkrbueQA52435140;     KYeCZGMugtLSVkrbueQA52435140 = KYeCZGMugtLSVkrbueQA83496461;     KYeCZGMugtLSVkrbueQA83496461 = KYeCZGMugtLSVkrbueQA6486857;     KYeCZGMugtLSVkrbueQA6486857 = KYeCZGMugtLSVkrbueQA51667102;     KYeCZGMugtLSVkrbueQA51667102 = KYeCZGMugtLSVkrbueQA72967193;     KYeCZGMugtLSVkrbueQA72967193 = KYeCZGMugtLSVkrbueQA21725077;     KYeCZGMugtLSVkrbueQA21725077 = KYeCZGMugtLSVkrbueQA625656;     KYeCZGMugtLSVkrbueQA625656 = KYeCZGMugtLSVkrbueQA48963834;     KYeCZGMugtLSVkrbueQA48963834 = KYeCZGMugtLSVkrbueQA55616716;     KYeCZGMugtLSVkrbueQA55616716 = KYeCZGMugtLSVkrbueQA78512788;     KYeCZGMugtLSVkrbueQA78512788 = KYeCZGMugtLSVkrbueQA12859613;     KYeCZGMugtLSVkrbueQA12859613 = KYeCZGMugtLSVkrbueQA26181410;     KYeCZGMugtLSVkrbueQA26181410 = KYeCZGMugtLSVkrbueQA87657155;     KYeCZGMugtLSVkrbueQA87657155 = KYeCZGMugtLSVkrbueQA60947989;     KYeCZGMugtLSVkrbueQA60947989 = KYeCZGMugtLSVkrbueQA87545992;     KYeCZGMugtLSVkrbueQA87545992 = KYeCZGMugtLSVkrbueQA92381044;     KYeCZGMugtLSVkrbueQA92381044 = KYeCZGMugtLSVkrbueQA81526317;     KYeCZGMugtLSVkrbueQA81526317 = KYeCZGMugtLSVkrbueQA41160641;     KYeCZGMugtLSVkrbueQA41160641 = KYeCZGMugtLSVkrbueQA4637219;     KYeCZGMugtLSVkrbueQA4637219 = KYeCZGMugtLSVkrbueQA30935867;     KYeCZGMugtLSVkrbueQA30935867 = KYeCZGMugtLSVkrbueQA5989178;     KYeCZGMugtLSVkrbueQA5989178 = KYeCZGMugtLSVkrbueQA87279551;     KYeCZGMugtLSVkrbueQA87279551 = KYeCZGMugtLSVkrbueQA59944130;     KYeCZGMugtLSVkrbueQA59944130 = KYeCZGMugtLSVkrbueQA55783685;     KYeCZGMugtLSVkrbueQA55783685 = KYeCZGMugtLSVkrbueQA17747484;     KYeCZGMugtLSVkrbueQA17747484 = KYeCZGMugtLSVkrbueQA5921830;     KYeCZGMugtLSVkrbueQA5921830 = KYeCZGMugtLSVkrbueQA38241871;     KYeCZGMugtLSVkrbueQA38241871 = KYeCZGMugtLSVkrbueQA3081529;     KYeCZGMugtLSVkrbueQA3081529 = KYeCZGMugtLSVkrbueQA16880763;     KYeCZGMugtLSVkrbueQA16880763 = KYeCZGMugtLSVkrbueQA78857821;     KYeCZGMugtLSVkrbueQA78857821 = KYeCZGMugtLSVkrbueQA12901995;     KYeCZGMugtLSVkrbueQA12901995 = KYeCZGMugtLSVkrbueQA15917849;     KYeCZGMugtLSVkrbueQA15917849 = KYeCZGMugtLSVkrbueQA60932770;     KYeCZGMugtLSVkrbueQA60932770 = KYeCZGMugtLSVkrbueQA35523780;     KYeCZGMugtLSVkrbueQA35523780 = KYeCZGMugtLSVkrbueQA78197239;     KYeCZGMugtLSVkrbueQA78197239 = KYeCZGMugtLSVkrbueQA61833201;     KYeCZGMugtLSVkrbueQA61833201 = KYeCZGMugtLSVkrbueQA90579970;     KYeCZGMugtLSVkrbueQA90579970 = KYeCZGMugtLSVkrbueQA73699240;     KYeCZGMugtLSVkrbueQA73699240 = KYeCZGMugtLSVkrbueQA17170493;     KYeCZGMugtLSVkrbueQA17170493 = KYeCZGMugtLSVkrbueQA92977812;     KYeCZGMugtLSVkrbueQA92977812 = KYeCZGMugtLSVkrbueQA9980970;     KYeCZGMugtLSVkrbueQA9980970 = KYeCZGMugtLSVkrbueQA23865439;     KYeCZGMugtLSVkrbueQA23865439 = KYeCZGMugtLSVkrbueQA54006124;     KYeCZGMugtLSVkrbueQA54006124 = KYeCZGMugtLSVkrbueQA59568095;     KYeCZGMugtLSVkrbueQA59568095 = KYeCZGMugtLSVkrbueQA85954507;     KYeCZGMugtLSVkrbueQA85954507 = KYeCZGMugtLSVkrbueQA71837887;     KYeCZGMugtLSVkrbueQA71837887 = KYeCZGMugtLSVkrbueQA67878692;     KYeCZGMugtLSVkrbueQA67878692 = KYeCZGMugtLSVkrbueQA56601959;     KYeCZGMugtLSVkrbueQA56601959 = KYeCZGMugtLSVkrbueQA677305;     KYeCZGMugtLSVkrbueQA677305 = KYeCZGMugtLSVkrbueQA92434905;     KYeCZGMugtLSVkrbueQA92434905 = KYeCZGMugtLSVkrbueQA17633014;     KYeCZGMugtLSVkrbueQA17633014 = KYeCZGMugtLSVkrbueQA77372976;     KYeCZGMugtLSVkrbueQA77372976 = KYeCZGMugtLSVkrbueQA98822674;     KYeCZGMugtLSVkrbueQA98822674 = KYeCZGMugtLSVkrbueQA42749778;     KYeCZGMugtLSVkrbueQA42749778 = KYeCZGMugtLSVkrbueQA3767336;     KYeCZGMugtLSVkrbueQA3767336 = KYeCZGMugtLSVkrbueQA65155589;     KYeCZGMugtLSVkrbueQA65155589 = KYeCZGMugtLSVkrbueQA23552331;     KYeCZGMugtLSVkrbueQA23552331 = KYeCZGMugtLSVkrbueQA50703171;     KYeCZGMugtLSVkrbueQA50703171 = KYeCZGMugtLSVkrbueQA33919618;     KYeCZGMugtLSVkrbueQA33919618 = KYeCZGMugtLSVkrbueQA67045363;     KYeCZGMugtLSVkrbueQA67045363 = KYeCZGMugtLSVkrbueQA83483205;     KYeCZGMugtLSVkrbueQA83483205 = KYeCZGMugtLSVkrbueQA97544127;     KYeCZGMugtLSVkrbueQA97544127 = KYeCZGMugtLSVkrbueQA32083071;     KYeCZGMugtLSVkrbueQA32083071 = KYeCZGMugtLSVkrbueQA76758895;     KYeCZGMugtLSVkrbueQA76758895 = KYeCZGMugtLSVkrbueQA65610793;     KYeCZGMugtLSVkrbueQA65610793 = KYeCZGMugtLSVkrbueQA96941764;     KYeCZGMugtLSVkrbueQA96941764 = KYeCZGMugtLSVkrbueQA65248639;     KYeCZGMugtLSVkrbueQA65248639 = KYeCZGMugtLSVkrbueQA52133376;     KYeCZGMugtLSVkrbueQA52133376 = KYeCZGMugtLSVkrbueQA82750750;     KYeCZGMugtLSVkrbueQA82750750 = KYeCZGMugtLSVkrbueQA25712791;     KYeCZGMugtLSVkrbueQA25712791 = KYeCZGMugtLSVkrbueQA1801075;     KYeCZGMugtLSVkrbueQA1801075 = KYeCZGMugtLSVkrbueQA7827078;     KYeCZGMugtLSVkrbueQA7827078 = KYeCZGMugtLSVkrbueQA23990149;     KYeCZGMugtLSVkrbueQA23990149 = KYeCZGMugtLSVkrbueQA11659406;     KYeCZGMugtLSVkrbueQA11659406 = KYeCZGMugtLSVkrbueQA20954897;     KYeCZGMugtLSVkrbueQA20954897 = KYeCZGMugtLSVkrbueQA82123739;     KYeCZGMugtLSVkrbueQA82123739 = KYeCZGMugtLSVkrbueQA33273428;     KYeCZGMugtLSVkrbueQA33273428 = KYeCZGMugtLSVkrbueQA376036;     KYeCZGMugtLSVkrbueQA376036 = KYeCZGMugtLSVkrbueQA69829177;     KYeCZGMugtLSVkrbueQA69829177 = KYeCZGMugtLSVkrbueQA45909597;     KYeCZGMugtLSVkrbueQA45909597 = KYeCZGMugtLSVkrbueQA38043138;     KYeCZGMugtLSVkrbueQA38043138 = KYeCZGMugtLSVkrbueQA81639912;     KYeCZGMugtLSVkrbueQA81639912 = KYeCZGMugtLSVkrbueQA2404225;     KYeCZGMugtLSVkrbueQA2404225 = KYeCZGMugtLSVkrbueQA24445857;     KYeCZGMugtLSVkrbueQA24445857 = KYeCZGMugtLSVkrbueQA61224808;     KYeCZGMugtLSVkrbueQA61224808 = KYeCZGMugtLSVkrbueQA35529019;     KYeCZGMugtLSVkrbueQA35529019 = KYeCZGMugtLSVkrbueQA17095174;     KYeCZGMugtLSVkrbueQA17095174 = KYeCZGMugtLSVkrbueQA18182992;     KYeCZGMugtLSVkrbueQA18182992 = KYeCZGMugtLSVkrbueQA31756444;     KYeCZGMugtLSVkrbueQA31756444 = KYeCZGMugtLSVkrbueQA13041650;     KYeCZGMugtLSVkrbueQA13041650 = KYeCZGMugtLSVkrbueQA38280871;     KYeCZGMugtLSVkrbueQA38280871 = KYeCZGMugtLSVkrbueQA39876799;     KYeCZGMugtLSVkrbueQA39876799 = KYeCZGMugtLSVkrbueQA17549949;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void JFmwmbOAdHRdevobRSaM9952061() {     float JJOIvggHpaxXareQisGl88201615 = -80573283;    float JJOIvggHpaxXareQisGl56896814 = -320266884;    float JJOIvggHpaxXareQisGl64784869 = -237853783;    float JJOIvggHpaxXareQisGl39090618 = 46666871;    float JJOIvggHpaxXareQisGl39983938 = -873800301;    float JJOIvggHpaxXareQisGl93459655 = -951861643;    float JJOIvggHpaxXareQisGl92064540 = 34754608;    float JJOIvggHpaxXareQisGl66598893 = -367676916;    float JJOIvggHpaxXareQisGl98300194 = 79134396;    float JJOIvggHpaxXareQisGl6462922 = -27896870;    float JJOIvggHpaxXareQisGl48736017 = -222538634;    float JJOIvggHpaxXareQisGl97213672 = -491154150;    float JJOIvggHpaxXareQisGl52794581 = 87435085;    float JJOIvggHpaxXareQisGl5572111 = -693162920;    float JJOIvggHpaxXareQisGl83345675 = -562538901;    float JJOIvggHpaxXareQisGl81492474 = -711187627;    float JJOIvggHpaxXareQisGl49469543 = -669441439;    float JJOIvggHpaxXareQisGl98806282 = -751086632;    float JJOIvggHpaxXareQisGl94567394 = -256484165;    float JJOIvggHpaxXareQisGl84968118 = -187427058;    float JJOIvggHpaxXareQisGl42792910 = -19065209;    float JJOIvggHpaxXareQisGl22343754 = -363175081;    float JJOIvggHpaxXareQisGl89489801 = -148818662;    float JJOIvggHpaxXareQisGl26230582 = -572124674;    float JJOIvggHpaxXareQisGl45650706 = -729365752;    float JJOIvggHpaxXareQisGl71758221 = 85724903;    float JJOIvggHpaxXareQisGl12898005 = -124302810;    float JJOIvggHpaxXareQisGl38718092 = -74224256;    float JJOIvggHpaxXareQisGl38131665 = -416045723;    float JJOIvggHpaxXareQisGl26451647 = 59327048;    float JJOIvggHpaxXareQisGl67016127 = -524287436;    float JJOIvggHpaxXareQisGl295146 = -561497054;    float JJOIvggHpaxXareQisGl692845 = 14789517;    float JJOIvggHpaxXareQisGl71286805 = -982362650;    float JJOIvggHpaxXareQisGl16266108 = -98424773;    float JJOIvggHpaxXareQisGl63074787 = -266711332;    float JJOIvggHpaxXareQisGl60672249 = -64780423;    float JJOIvggHpaxXareQisGl31650310 = -690414502;    float JJOIvggHpaxXareQisGl91435283 = 38026443;    float JJOIvggHpaxXareQisGl11021147 = 98679831;    float JJOIvggHpaxXareQisGl21006054 = -213412806;    float JJOIvggHpaxXareQisGl64006771 = -980408291;    float JJOIvggHpaxXareQisGl27164199 = -554230210;    float JJOIvggHpaxXareQisGl68341502 = -826698939;    float JJOIvggHpaxXareQisGl86534028 = -122970660;    float JJOIvggHpaxXareQisGl80105075 = -456500333;    float JJOIvggHpaxXareQisGl43301695 = -744137514;    float JJOIvggHpaxXareQisGl58592023 = -886522702;    float JJOIvggHpaxXareQisGl15422422 = -82155862;    float JJOIvggHpaxXareQisGl32292925 = -953757926;    float JJOIvggHpaxXareQisGl22195448 = -586302899;    float JJOIvggHpaxXareQisGl1439600 = -857978403;    float JJOIvggHpaxXareQisGl41030165 = -35043673;    float JJOIvggHpaxXareQisGl62065192 = -394731169;    float JJOIvggHpaxXareQisGl67159151 = -461910062;    float JJOIvggHpaxXareQisGl65857861 = -717398203;    float JJOIvggHpaxXareQisGl67407013 = -71448223;    float JJOIvggHpaxXareQisGl38554288 = -665729109;    float JJOIvggHpaxXareQisGl93439911 = -223967378;    float JJOIvggHpaxXareQisGl68225717 = -859525204;    float JJOIvggHpaxXareQisGl80561651 = -727558834;    float JJOIvggHpaxXareQisGl53346448 = -891021137;    float JJOIvggHpaxXareQisGl28467229 = -951631193;    float JJOIvggHpaxXareQisGl71848547 = -980192652;    float JJOIvggHpaxXareQisGl39446795 = -503609434;    float JJOIvggHpaxXareQisGl48440871 = -661041580;    float JJOIvggHpaxXareQisGl96520827 = -405943667;    float JJOIvggHpaxXareQisGl81507775 = 69797734;    float JJOIvggHpaxXareQisGl89306003 = -494738147;    float JJOIvggHpaxXareQisGl20270888 = -195827570;    float JJOIvggHpaxXareQisGl20820225 = -546407204;    float JJOIvggHpaxXareQisGl17819234 = -979026938;    float JJOIvggHpaxXareQisGl7370999 = -689113075;    float JJOIvggHpaxXareQisGl83546247 = -255163997;    float JJOIvggHpaxXareQisGl63962065 = -974014252;    float JJOIvggHpaxXareQisGl78786139 = -38656918;    float JJOIvggHpaxXareQisGl95179555 = -808944871;    float JJOIvggHpaxXareQisGl21148300 = -322119723;    float JJOIvggHpaxXareQisGl39696553 = -349154014;    float JJOIvggHpaxXareQisGl65545631 = -172865419;    float JJOIvggHpaxXareQisGl28456526 = -170137584;    float JJOIvggHpaxXareQisGl54305981 = -237780109;    float JJOIvggHpaxXareQisGl23295670 = -992068395;    float JJOIvggHpaxXareQisGl5838741 = -462287798;    float JJOIvggHpaxXareQisGl4256199 = -354370053;    float JJOIvggHpaxXareQisGl65576527 = -666309033;    float JJOIvggHpaxXareQisGl59264981 = -426453382;    float JJOIvggHpaxXareQisGl38627653 = -590479314;    float JJOIvggHpaxXareQisGl4127654 = -420452589;    float JJOIvggHpaxXareQisGl50408246 = -381026570;    float JJOIvggHpaxXareQisGl95667774 = -95263109;    float JJOIvggHpaxXareQisGl22117961 = -399051315;    float JJOIvggHpaxXareQisGl38210398 = -366447124;    float JJOIvggHpaxXareQisGl23209567 = -102448354;    float JJOIvggHpaxXareQisGl30459495 = -173761335;    float JJOIvggHpaxXareQisGl67659606 = -322391669;    float JJOIvggHpaxXareQisGl35539543 = 71222901;    float JJOIvggHpaxXareQisGl55315651 = -574037558;    float JJOIvggHpaxXareQisGl28894707 = -223089505;    float JJOIvggHpaxXareQisGl38093157 = -80573283;     JJOIvggHpaxXareQisGl88201615 = JJOIvggHpaxXareQisGl56896814;     JJOIvggHpaxXareQisGl56896814 = JJOIvggHpaxXareQisGl64784869;     JJOIvggHpaxXareQisGl64784869 = JJOIvggHpaxXareQisGl39090618;     JJOIvggHpaxXareQisGl39090618 = JJOIvggHpaxXareQisGl39983938;     JJOIvggHpaxXareQisGl39983938 = JJOIvggHpaxXareQisGl93459655;     JJOIvggHpaxXareQisGl93459655 = JJOIvggHpaxXareQisGl92064540;     JJOIvggHpaxXareQisGl92064540 = JJOIvggHpaxXareQisGl66598893;     JJOIvggHpaxXareQisGl66598893 = JJOIvggHpaxXareQisGl98300194;     JJOIvggHpaxXareQisGl98300194 = JJOIvggHpaxXareQisGl6462922;     JJOIvggHpaxXareQisGl6462922 = JJOIvggHpaxXareQisGl48736017;     JJOIvggHpaxXareQisGl48736017 = JJOIvggHpaxXareQisGl97213672;     JJOIvggHpaxXareQisGl97213672 = JJOIvggHpaxXareQisGl52794581;     JJOIvggHpaxXareQisGl52794581 = JJOIvggHpaxXareQisGl5572111;     JJOIvggHpaxXareQisGl5572111 = JJOIvggHpaxXareQisGl83345675;     JJOIvggHpaxXareQisGl83345675 = JJOIvggHpaxXareQisGl81492474;     JJOIvggHpaxXareQisGl81492474 = JJOIvggHpaxXareQisGl49469543;     JJOIvggHpaxXareQisGl49469543 = JJOIvggHpaxXareQisGl98806282;     JJOIvggHpaxXareQisGl98806282 = JJOIvggHpaxXareQisGl94567394;     JJOIvggHpaxXareQisGl94567394 = JJOIvggHpaxXareQisGl84968118;     JJOIvggHpaxXareQisGl84968118 = JJOIvggHpaxXareQisGl42792910;     JJOIvggHpaxXareQisGl42792910 = JJOIvggHpaxXareQisGl22343754;     JJOIvggHpaxXareQisGl22343754 = JJOIvggHpaxXareQisGl89489801;     JJOIvggHpaxXareQisGl89489801 = JJOIvggHpaxXareQisGl26230582;     JJOIvggHpaxXareQisGl26230582 = JJOIvggHpaxXareQisGl45650706;     JJOIvggHpaxXareQisGl45650706 = JJOIvggHpaxXareQisGl71758221;     JJOIvggHpaxXareQisGl71758221 = JJOIvggHpaxXareQisGl12898005;     JJOIvggHpaxXareQisGl12898005 = JJOIvggHpaxXareQisGl38718092;     JJOIvggHpaxXareQisGl38718092 = JJOIvggHpaxXareQisGl38131665;     JJOIvggHpaxXareQisGl38131665 = JJOIvggHpaxXareQisGl26451647;     JJOIvggHpaxXareQisGl26451647 = JJOIvggHpaxXareQisGl67016127;     JJOIvggHpaxXareQisGl67016127 = JJOIvggHpaxXareQisGl295146;     JJOIvggHpaxXareQisGl295146 = JJOIvggHpaxXareQisGl692845;     JJOIvggHpaxXareQisGl692845 = JJOIvggHpaxXareQisGl71286805;     JJOIvggHpaxXareQisGl71286805 = JJOIvggHpaxXareQisGl16266108;     JJOIvggHpaxXareQisGl16266108 = JJOIvggHpaxXareQisGl63074787;     JJOIvggHpaxXareQisGl63074787 = JJOIvggHpaxXareQisGl60672249;     JJOIvggHpaxXareQisGl60672249 = JJOIvggHpaxXareQisGl31650310;     JJOIvggHpaxXareQisGl31650310 = JJOIvggHpaxXareQisGl91435283;     JJOIvggHpaxXareQisGl91435283 = JJOIvggHpaxXareQisGl11021147;     JJOIvggHpaxXareQisGl11021147 = JJOIvggHpaxXareQisGl21006054;     JJOIvggHpaxXareQisGl21006054 = JJOIvggHpaxXareQisGl64006771;     JJOIvggHpaxXareQisGl64006771 = JJOIvggHpaxXareQisGl27164199;     JJOIvggHpaxXareQisGl27164199 = JJOIvggHpaxXareQisGl68341502;     JJOIvggHpaxXareQisGl68341502 = JJOIvggHpaxXareQisGl86534028;     JJOIvggHpaxXareQisGl86534028 = JJOIvggHpaxXareQisGl80105075;     JJOIvggHpaxXareQisGl80105075 = JJOIvggHpaxXareQisGl43301695;     JJOIvggHpaxXareQisGl43301695 = JJOIvggHpaxXareQisGl58592023;     JJOIvggHpaxXareQisGl58592023 = JJOIvggHpaxXareQisGl15422422;     JJOIvggHpaxXareQisGl15422422 = JJOIvggHpaxXareQisGl32292925;     JJOIvggHpaxXareQisGl32292925 = JJOIvggHpaxXareQisGl22195448;     JJOIvggHpaxXareQisGl22195448 = JJOIvggHpaxXareQisGl1439600;     JJOIvggHpaxXareQisGl1439600 = JJOIvggHpaxXareQisGl41030165;     JJOIvggHpaxXareQisGl41030165 = JJOIvggHpaxXareQisGl62065192;     JJOIvggHpaxXareQisGl62065192 = JJOIvggHpaxXareQisGl67159151;     JJOIvggHpaxXareQisGl67159151 = JJOIvggHpaxXareQisGl65857861;     JJOIvggHpaxXareQisGl65857861 = JJOIvggHpaxXareQisGl67407013;     JJOIvggHpaxXareQisGl67407013 = JJOIvggHpaxXareQisGl38554288;     JJOIvggHpaxXareQisGl38554288 = JJOIvggHpaxXareQisGl93439911;     JJOIvggHpaxXareQisGl93439911 = JJOIvggHpaxXareQisGl68225717;     JJOIvggHpaxXareQisGl68225717 = JJOIvggHpaxXareQisGl80561651;     JJOIvggHpaxXareQisGl80561651 = JJOIvggHpaxXareQisGl53346448;     JJOIvggHpaxXareQisGl53346448 = JJOIvggHpaxXareQisGl28467229;     JJOIvggHpaxXareQisGl28467229 = JJOIvggHpaxXareQisGl71848547;     JJOIvggHpaxXareQisGl71848547 = JJOIvggHpaxXareQisGl39446795;     JJOIvggHpaxXareQisGl39446795 = JJOIvggHpaxXareQisGl48440871;     JJOIvggHpaxXareQisGl48440871 = JJOIvggHpaxXareQisGl96520827;     JJOIvggHpaxXareQisGl96520827 = JJOIvggHpaxXareQisGl81507775;     JJOIvggHpaxXareQisGl81507775 = JJOIvggHpaxXareQisGl89306003;     JJOIvggHpaxXareQisGl89306003 = JJOIvggHpaxXareQisGl20270888;     JJOIvggHpaxXareQisGl20270888 = JJOIvggHpaxXareQisGl20820225;     JJOIvggHpaxXareQisGl20820225 = JJOIvggHpaxXareQisGl17819234;     JJOIvggHpaxXareQisGl17819234 = JJOIvggHpaxXareQisGl7370999;     JJOIvggHpaxXareQisGl7370999 = JJOIvggHpaxXareQisGl83546247;     JJOIvggHpaxXareQisGl83546247 = JJOIvggHpaxXareQisGl63962065;     JJOIvggHpaxXareQisGl63962065 = JJOIvggHpaxXareQisGl78786139;     JJOIvggHpaxXareQisGl78786139 = JJOIvggHpaxXareQisGl95179555;     JJOIvggHpaxXareQisGl95179555 = JJOIvggHpaxXareQisGl21148300;     JJOIvggHpaxXareQisGl21148300 = JJOIvggHpaxXareQisGl39696553;     JJOIvggHpaxXareQisGl39696553 = JJOIvggHpaxXareQisGl65545631;     JJOIvggHpaxXareQisGl65545631 = JJOIvggHpaxXareQisGl28456526;     JJOIvggHpaxXareQisGl28456526 = JJOIvggHpaxXareQisGl54305981;     JJOIvggHpaxXareQisGl54305981 = JJOIvggHpaxXareQisGl23295670;     JJOIvggHpaxXareQisGl23295670 = JJOIvggHpaxXareQisGl5838741;     JJOIvggHpaxXareQisGl5838741 = JJOIvggHpaxXareQisGl4256199;     JJOIvggHpaxXareQisGl4256199 = JJOIvggHpaxXareQisGl65576527;     JJOIvggHpaxXareQisGl65576527 = JJOIvggHpaxXareQisGl59264981;     JJOIvggHpaxXareQisGl59264981 = JJOIvggHpaxXareQisGl38627653;     JJOIvggHpaxXareQisGl38627653 = JJOIvggHpaxXareQisGl4127654;     JJOIvggHpaxXareQisGl4127654 = JJOIvggHpaxXareQisGl50408246;     JJOIvggHpaxXareQisGl50408246 = JJOIvggHpaxXareQisGl95667774;     JJOIvggHpaxXareQisGl95667774 = JJOIvggHpaxXareQisGl22117961;     JJOIvggHpaxXareQisGl22117961 = JJOIvggHpaxXareQisGl38210398;     JJOIvggHpaxXareQisGl38210398 = JJOIvggHpaxXareQisGl23209567;     JJOIvggHpaxXareQisGl23209567 = JJOIvggHpaxXareQisGl30459495;     JJOIvggHpaxXareQisGl30459495 = JJOIvggHpaxXareQisGl67659606;     JJOIvggHpaxXareQisGl67659606 = JJOIvggHpaxXareQisGl35539543;     JJOIvggHpaxXareQisGl35539543 = JJOIvggHpaxXareQisGl55315651;     JJOIvggHpaxXareQisGl55315651 = JJOIvggHpaxXareQisGl28894707;     JJOIvggHpaxXareQisGl28894707 = JJOIvggHpaxXareQisGl38093157;     JJOIvggHpaxXareQisGl38093157 = JJOIvggHpaxXareQisGl88201615;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void BvdZVjRjguuzpuDrxrsz16610806() {     float AEPfAMhxldgbsXNbxtRt88195144 = 46902575;    float AEPfAMhxldgbsXNbxtRt95469130 = -362862009;    float AEPfAMhxldgbsXNbxtRt66098081 = -194982396;    float AEPfAMhxldgbsXNbxtRt24170479 = -873873959;    float AEPfAMhxldgbsXNbxtRt48824040 = -242155622;    float AEPfAMhxldgbsXNbxtRt28265088 = -651294455;    float AEPfAMhxldgbsXNbxtRt60611876 = -769781859;    float AEPfAMhxldgbsXNbxtRt35812944 = -71854233;    float AEPfAMhxldgbsXNbxtRt81040358 = -734511674;    float AEPfAMhxldgbsXNbxtRt3102436 = -764703741;    float AEPfAMhxldgbsXNbxtRt69056622 = -182010706;    float AEPfAMhxldgbsXNbxtRt11505350 = -226632512;    float AEPfAMhxldgbsXNbxtRt84647494 = 65776610;    float AEPfAMhxldgbsXNbxtRt59416331 = 61684029;    float AEPfAMhxldgbsXNbxtRt28517784 = -525251753;    float AEPfAMhxldgbsXNbxtRt56057021 = 56016722;    float AEPfAMhxldgbsXNbxtRt5322303 = -489056009;    float AEPfAMhxldgbsXNbxtRt21806816 = -48158253;    float AEPfAMhxldgbsXNbxtRt14995665 = -874466432;    float AEPfAMhxldgbsXNbxtRt57041882 = -799451509;    float AEPfAMhxldgbsXNbxtRt22462334 = -829531583;    float AEPfAMhxldgbsXNbxtRt640021 = -873166964;    float AEPfAMhxldgbsXNbxtRt26368198 = -607776504;    float AEPfAMhxldgbsXNbxtRt69880690 = -453045196;    float AEPfAMhxldgbsXNbxtRt74093396 = -56059605;    float AEPfAMhxldgbsXNbxtRt88332298 = -922481026;    float AEPfAMhxldgbsXNbxtRt83119586 = -425102523;    float AEPfAMhxldgbsXNbxtRt74121616 = -683191244;    float AEPfAMhxldgbsXNbxtRt50810112 = 37883962;    float AEPfAMhxldgbsXNbxtRt2541517 = -716104326;    float AEPfAMhxldgbsXNbxtRt428806 = -911380670;    float AEPfAMhxldgbsXNbxtRt85019003 = -953318816;    float AEPfAMhxldgbsXNbxtRt86145268 = -187154699;    float AEPfAMhxldgbsXNbxtRt71569642 = -859044383;    float AEPfAMhxldgbsXNbxtRt98624145 = -357738698;    float AEPfAMhxldgbsXNbxtRt5902977 = -346477078;    float AEPfAMhxldgbsXNbxtRt72613616 = -164433802;    float AEPfAMhxldgbsXNbxtRt1567990 = -685677181;    float AEPfAMhxldgbsXNbxtRt26748881 = -674654973;    float AEPfAMhxldgbsXNbxtRt82894039 = -610751855;    float AEPfAMhxldgbsXNbxtRt8861722 = -301356704;    float AEPfAMhxldgbsXNbxtRt49704432 = -74200373;    float AEPfAMhxldgbsXNbxtRt87447558 = -13360465;    float AEPfAMhxldgbsXNbxtRt27573400 = -383663382;    float AEPfAMhxldgbsXNbxtRt70552081 = -596230062;    float AEPfAMhxldgbsXNbxtRt20533043 = -268545004;    float AEPfAMhxldgbsXNbxtRt70999488 = -125418136;    float AEPfAMhxldgbsXNbxtRt33975604 = -395284058;    float AEPfAMhxldgbsXNbxtRt34080811 = -745919723;    float AEPfAMhxldgbsXNbxtRt52338348 = -540782813;    float AEPfAMhxldgbsXNbxtRt35454818 = -164857906;    float AEPfAMhxldgbsXNbxtRt15986857 = -140124151;    float AEPfAMhxldgbsXNbxtRt16478513 = 72902319;    float AEPfAMhxldgbsXNbxtRt72315462 = -926651020;    float AEPfAMhxldgbsXNbxtRt94148471 = -218809339;    float AEPfAMhxldgbsXNbxtRt87555124 = -79930462;    float AEPfAMhxldgbsXNbxtRt69100933 = -755085506;    float AEPfAMhxldgbsXNbxtRt96217391 = -741937200;    float AEPfAMhxldgbsXNbxtRt50077083 = -717814355;    float AEPfAMhxldgbsXNbxtRt60491742 = -319674597;    float AEPfAMhxldgbsXNbxtRt45145502 = -126191933;    float AEPfAMhxldgbsXNbxtRt86490259 = 13409384;    float AEPfAMhxldgbsXNbxtRt85002832 = -9738195;    float AEPfAMhxldgbsXNbxtRt78498841 = 81592651;    float AEPfAMhxldgbsXNbxtRt2673630 = -853323071;    float AEPfAMhxldgbsXNbxtRt84037619 = -228691890;    float AEPfAMhxldgbsXNbxtRt25360082 = 60522186;    float AEPfAMhxldgbsXNbxtRt13077853 = -75179008;    float AEPfAMhxldgbsXNbxtRt60792185 = -580577274;    float AEPfAMhxldgbsXNbxtRt22614808 = -78774676;    float AEPfAMhxldgbsXNbxtRt83443404 = -779549477;    float AEPfAMhxldgbsXNbxtRt3754314 = -803378829;    float AEPfAMhxldgbsXNbxtRt95057934 = -373503281;    float AEPfAMhxldgbsXNbxtRt32101625 = -163714578;    float AEPfAMhxldgbsXNbxtRt48180160 = -398094805;    float AEPfAMhxldgbsXNbxtRt72757902 = -655331210;    float AEPfAMhxldgbsXNbxtRt13192462 = -759806500;    float AEPfAMhxldgbsXNbxtRt98794797 = -124113122;    float AEPfAMhxldgbsXNbxtRt99328608 = -856815135;    float AEPfAMhxldgbsXNbxtRt53560353 = -787514601;    float AEPfAMhxldgbsXNbxtRt17332811 = -697062890;    float AEPfAMhxldgbsXNbxtRt49143982 = 70181534;    float AEPfAMhxldgbsXNbxtRt40040806 = -937271522;    float AEPfAMhxldgbsXNbxtRt98471764 = -421333226;    float AEPfAMhxldgbsXNbxtRt67086699 = -451246420;    float AEPfAMhxldgbsXNbxtRt84441949 = -671256520;    float AEPfAMhxldgbsXNbxtRt68540490 = -926221135;    float AEPfAMhxldgbsXNbxtRt13829806 = -260503679;    float AEPfAMhxldgbsXNbxtRt77421170 = -540235044;    float AEPfAMhxldgbsXNbxtRt11069022 = -177808237;    float AEPfAMhxldgbsXNbxtRt36802044 = -591391572;    float AEPfAMhxldgbsXNbxtRt76396225 = -422496602;    float AEPfAMhxldgbsXNbxtRt51490907 = -967862826;    float AEPfAMhxldgbsXNbxtRt66257138 = -254980377;    float AEPfAMhxldgbsXNbxtRt37748537 = -384559923;    float AEPfAMhxldgbsXNbxtRt22371463 = -214766089;    float AEPfAMhxldgbsXNbxtRt64701600 = 35537821;    float AEPfAMhxldgbsXNbxtRt8948717 = 5046884;    float AEPfAMhxldgbsXNbxtRt24899770 = -530340312;    float AEPfAMhxldgbsXNbxtRt86514461 = 46902575;     AEPfAMhxldgbsXNbxtRt88195144 = AEPfAMhxldgbsXNbxtRt95469130;     AEPfAMhxldgbsXNbxtRt95469130 = AEPfAMhxldgbsXNbxtRt66098081;     AEPfAMhxldgbsXNbxtRt66098081 = AEPfAMhxldgbsXNbxtRt24170479;     AEPfAMhxldgbsXNbxtRt24170479 = AEPfAMhxldgbsXNbxtRt48824040;     AEPfAMhxldgbsXNbxtRt48824040 = AEPfAMhxldgbsXNbxtRt28265088;     AEPfAMhxldgbsXNbxtRt28265088 = AEPfAMhxldgbsXNbxtRt60611876;     AEPfAMhxldgbsXNbxtRt60611876 = AEPfAMhxldgbsXNbxtRt35812944;     AEPfAMhxldgbsXNbxtRt35812944 = AEPfAMhxldgbsXNbxtRt81040358;     AEPfAMhxldgbsXNbxtRt81040358 = AEPfAMhxldgbsXNbxtRt3102436;     AEPfAMhxldgbsXNbxtRt3102436 = AEPfAMhxldgbsXNbxtRt69056622;     AEPfAMhxldgbsXNbxtRt69056622 = AEPfAMhxldgbsXNbxtRt11505350;     AEPfAMhxldgbsXNbxtRt11505350 = AEPfAMhxldgbsXNbxtRt84647494;     AEPfAMhxldgbsXNbxtRt84647494 = AEPfAMhxldgbsXNbxtRt59416331;     AEPfAMhxldgbsXNbxtRt59416331 = AEPfAMhxldgbsXNbxtRt28517784;     AEPfAMhxldgbsXNbxtRt28517784 = AEPfAMhxldgbsXNbxtRt56057021;     AEPfAMhxldgbsXNbxtRt56057021 = AEPfAMhxldgbsXNbxtRt5322303;     AEPfAMhxldgbsXNbxtRt5322303 = AEPfAMhxldgbsXNbxtRt21806816;     AEPfAMhxldgbsXNbxtRt21806816 = AEPfAMhxldgbsXNbxtRt14995665;     AEPfAMhxldgbsXNbxtRt14995665 = AEPfAMhxldgbsXNbxtRt57041882;     AEPfAMhxldgbsXNbxtRt57041882 = AEPfAMhxldgbsXNbxtRt22462334;     AEPfAMhxldgbsXNbxtRt22462334 = AEPfAMhxldgbsXNbxtRt640021;     AEPfAMhxldgbsXNbxtRt640021 = AEPfAMhxldgbsXNbxtRt26368198;     AEPfAMhxldgbsXNbxtRt26368198 = AEPfAMhxldgbsXNbxtRt69880690;     AEPfAMhxldgbsXNbxtRt69880690 = AEPfAMhxldgbsXNbxtRt74093396;     AEPfAMhxldgbsXNbxtRt74093396 = AEPfAMhxldgbsXNbxtRt88332298;     AEPfAMhxldgbsXNbxtRt88332298 = AEPfAMhxldgbsXNbxtRt83119586;     AEPfAMhxldgbsXNbxtRt83119586 = AEPfAMhxldgbsXNbxtRt74121616;     AEPfAMhxldgbsXNbxtRt74121616 = AEPfAMhxldgbsXNbxtRt50810112;     AEPfAMhxldgbsXNbxtRt50810112 = AEPfAMhxldgbsXNbxtRt2541517;     AEPfAMhxldgbsXNbxtRt2541517 = AEPfAMhxldgbsXNbxtRt428806;     AEPfAMhxldgbsXNbxtRt428806 = AEPfAMhxldgbsXNbxtRt85019003;     AEPfAMhxldgbsXNbxtRt85019003 = AEPfAMhxldgbsXNbxtRt86145268;     AEPfAMhxldgbsXNbxtRt86145268 = AEPfAMhxldgbsXNbxtRt71569642;     AEPfAMhxldgbsXNbxtRt71569642 = AEPfAMhxldgbsXNbxtRt98624145;     AEPfAMhxldgbsXNbxtRt98624145 = AEPfAMhxldgbsXNbxtRt5902977;     AEPfAMhxldgbsXNbxtRt5902977 = AEPfAMhxldgbsXNbxtRt72613616;     AEPfAMhxldgbsXNbxtRt72613616 = AEPfAMhxldgbsXNbxtRt1567990;     AEPfAMhxldgbsXNbxtRt1567990 = AEPfAMhxldgbsXNbxtRt26748881;     AEPfAMhxldgbsXNbxtRt26748881 = AEPfAMhxldgbsXNbxtRt82894039;     AEPfAMhxldgbsXNbxtRt82894039 = AEPfAMhxldgbsXNbxtRt8861722;     AEPfAMhxldgbsXNbxtRt8861722 = AEPfAMhxldgbsXNbxtRt49704432;     AEPfAMhxldgbsXNbxtRt49704432 = AEPfAMhxldgbsXNbxtRt87447558;     AEPfAMhxldgbsXNbxtRt87447558 = AEPfAMhxldgbsXNbxtRt27573400;     AEPfAMhxldgbsXNbxtRt27573400 = AEPfAMhxldgbsXNbxtRt70552081;     AEPfAMhxldgbsXNbxtRt70552081 = AEPfAMhxldgbsXNbxtRt20533043;     AEPfAMhxldgbsXNbxtRt20533043 = AEPfAMhxldgbsXNbxtRt70999488;     AEPfAMhxldgbsXNbxtRt70999488 = AEPfAMhxldgbsXNbxtRt33975604;     AEPfAMhxldgbsXNbxtRt33975604 = AEPfAMhxldgbsXNbxtRt34080811;     AEPfAMhxldgbsXNbxtRt34080811 = AEPfAMhxldgbsXNbxtRt52338348;     AEPfAMhxldgbsXNbxtRt52338348 = AEPfAMhxldgbsXNbxtRt35454818;     AEPfAMhxldgbsXNbxtRt35454818 = AEPfAMhxldgbsXNbxtRt15986857;     AEPfAMhxldgbsXNbxtRt15986857 = AEPfAMhxldgbsXNbxtRt16478513;     AEPfAMhxldgbsXNbxtRt16478513 = AEPfAMhxldgbsXNbxtRt72315462;     AEPfAMhxldgbsXNbxtRt72315462 = AEPfAMhxldgbsXNbxtRt94148471;     AEPfAMhxldgbsXNbxtRt94148471 = AEPfAMhxldgbsXNbxtRt87555124;     AEPfAMhxldgbsXNbxtRt87555124 = AEPfAMhxldgbsXNbxtRt69100933;     AEPfAMhxldgbsXNbxtRt69100933 = AEPfAMhxldgbsXNbxtRt96217391;     AEPfAMhxldgbsXNbxtRt96217391 = AEPfAMhxldgbsXNbxtRt50077083;     AEPfAMhxldgbsXNbxtRt50077083 = AEPfAMhxldgbsXNbxtRt60491742;     AEPfAMhxldgbsXNbxtRt60491742 = AEPfAMhxldgbsXNbxtRt45145502;     AEPfAMhxldgbsXNbxtRt45145502 = AEPfAMhxldgbsXNbxtRt86490259;     AEPfAMhxldgbsXNbxtRt86490259 = AEPfAMhxldgbsXNbxtRt85002832;     AEPfAMhxldgbsXNbxtRt85002832 = AEPfAMhxldgbsXNbxtRt78498841;     AEPfAMhxldgbsXNbxtRt78498841 = AEPfAMhxldgbsXNbxtRt2673630;     AEPfAMhxldgbsXNbxtRt2673630 = AEPfAMhxldgbsXNbxtRt84037619;     AEPfAMhxldgbsXNbxtRt84037619 = AEPfAMhxldgbsXNbxtRt25360082;     AEPfAMhxldgbsXNbxtRt25360082 = AEPfAMhxldgbsXNbxtRt13077853;     AEPfAMhxldgbsXNbxtRt13077853 = AEPfAMhxldgbsXNbxtRt60792185;     AEPfAMhxldgbsXNbxtRt60792185 = AEPfAMhxldgbsXNbxtRt22614808;     AEPfAMhxldgbsXNbxtRt22614808 = AEPfAMhxldgbsXNbxtRt83443404;     AEPfAMhxldgbsXNbxtRt83443404 = AEPfAMhxldgbsXNbxtRt3754314;     AEPfAMhxldgbsXNbxtRt3754314 = AEPfAMhxldgbsXNbxtRt95057934;     AEPfAMhxldgbsXNbxtRt95057934 = AEPfAMhxldgbsXNbxtRt32101625;     AEPfAMhxldgbsXNbxtRt32101625 = AEPfAMhxldgbsXNbxtRt48180160;     AEPfAMhxldgbsXNbxtRt48180160 = AEPfAMhxldgbsXNbxtRt72757902;     AEPfAMhxldgbsXNbxtRt72757902 = AEPfAMhxldgbsXNbxtRt13192462;     AEPfAMhxldgbsXNbxtRt13192462 = AEPfAMhxldgbsXNbxtRt98794797;     AEPfAMhxldgbsXNbxtRt98794797 = AEPfAMhxldgbsXNbxtRt99328608;     AEPfAMhxldgbsXNbxtRt99328608 = AEPfAMhxldgbsXNbxtRt53560353;     AEPfAMhxldgbsXNbxtRt53560353 = AEPfAMhxldgbsXNbxtRt17332811;     AEPfAMhxldgbsXNbxtRt17332811 = AEPfAMhxldgbsXNbxtRt49143982;     AEPfAMhxldgbsXNbxtRt49143982 = AEPfAMhxldgbsXNbxtRt40040806;     AEPfAMhxldgbsXNbxtRt40040806 = AEPfAMhxldgbsXNbxtRt98471764;     AEPfAMhxldgbsXNbxtRt98471764 = AEPfAMhxldgbsXNbxtRt67086699;     AEPfAMhxldgbsXNbxtRt67086699 = AEPfAMhxldgbsXNbxtRt84441949;     AEPfAMhxldgbsXNbxtRt84441949 = AEPfAMhxldgbsXNbxtRt68540490;     AEPfAMhxldgbsXNbxtRt68540490 = AEPfAMhxldgbsXNbxtRt13829806;     AEPfAMhxldgbsXNbxtRt13829806 = AEPfAMhxldgbsXNbxtRt77421170;     AEPfAMhxldgbsXNbxtRt77421170 = AEPfAMhxldgbsXNbxtRt11069022;     AEPfAMhxldgbsXNbxtRt11069022 = AEPfAMhxldgbsXNbxtRt36802044;     AEPfAMhxldgbsXNbxtRt36802044 = AEPfAMhxldgbsXNbxtRt76396225;     AEPfAMhxldgbsXNbxtRt76396225 = AEPfAMhxldgbsXNbxtRt51490907;     AEPfAMhxldgbsXNbxtRt51490907 = AEPfAMhxldgbsXNbxtRt66257138;     AEPfAMhxldgbsXNbxtRt66257138 = AEPfAMhxldgbsXNbxtRt37748537;     AEPfAMhxldgbsXNbxtRt37748537 = AEPfAMhxldgbsXNbxtRt22371463;     AEPfAMhxldgbsXNbxtRt22371463 = AEPfAMhxldgbsXNbxtRt64701600;     AEPfAMhxldgbsXNbxtRt64701600 = AEPfAMhxldgbsXNbxtRt8948717;     AEPfAMhxldgbsXNbxtRt8948717 = AEPfAMhxldgbsXNbxtRt24899770;     AEPfAMhxldgbsXNbxtRt24899770 = AEPfAMhxldgbsXNbxtRt86514461;     AEPfAMhxldgbsXNbxtRt86514461 = AEPfAMhxldgbsXNbxtRt88195144;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void UNiFqeeRaUaHwgCaygUY71027019() {     float QMHCxuyOeYhxFyLqnukm58846811 = -571246555;    float QMHCxuyOeYhxFyLqnukm64142648 = -234505308;    float QMHCxuyOeYhxFyLqnukm46067001 = -637276856;    float QMHCxuyOeYhxFyLqnukm64101765 = 35987652;    float QMHCxuyOeYhxFyLqnukm70274361 = -25561969;    float QMHCxuyOeYhxFyLqnukm18264851 = -352875607;    float QMHCxuyOeYhxFyLqnukm78990771 = -768557569;    float QMHCxuyOeYhxFyLqnukm92655324 = -418671323;    float QMHCxuyOeYhxFyLqnukm26905413 = -827859002;    float QMHCxuyOeYhxFyLqnukm26068896 = 34200909;    float QMHCxuyOeYhxFyLqnukm11305784 = -216912635;    float QMHCxuyOeYhxFyLqnukm57051921 = -542396305;    float QMHCxuyOeYhxFyLqnukm64474881 = -943760448;    float QMHCxuyOeYhxFyLqnukm43263364 = -839344181;    float QMHCxuyOeYhxFyLqnukm11237804 = -675001756;    float QMHCxuyOeYhxFyLqnukm88585661 = -613966865;    float QMHCxuyOeYhxFyLqnukm99175129 = -398241139;    float QMHCxuyOeYhxFyLqnukm42100310 = -37324449;    float QMHCxuyOeYhxFyLqnukm96703446 = -481654374;    float QMHCxuyOeYhxFyLqnukm15828591 = -241817814;    float QMHCxuyOeYhxFyLqnukm77598088 = -738210723;    float QMHCxuyOeYhxFyLqnukm62035785 = -104452269;    float QMHCxuyOeYhxFyLqnukm28312006 = -75619648;    float QMHCxuyOeYhxFyLqnukm3730227 = -45446103;    float QMHCxuyOeYhxFyLqnukm38217784 = -831964292;    float QMHCxuyOeYhxFyLqnukm18929879 = 79234530;    float QMHCxuyOeYhxFyLqnukm91380371 = -820280298;    float QMHCxuyOeYhxFyLqnukm81903842 = -88315012;    float QMHCxuyOeYhxFyLqnukm82952600 = -256474255;    float QMHCxuyOeYhxFyLqnukm41713613 = -174499396;    float QMHCxuyOeYhxFyLqnukm7500802 = -659814356;    float QMHCxuyOeYhxFyLqnukm29530464 = -181615979;    float QMHCxuyOeYhxFyLqnukm69090629 = -134116308;    float QMHCxuyOeYhxFyLqnukm36934617 = -422574067;    float QMHCxuyOeYhxFyLqnukm76648382 = -204685477;    float QMHCxuyOeYhxFyLqnukm65896236 = -425703626;    float QMHCxuyOeYhxFyLqnukm16405103 = -353027009;    float QMHCxuyOeYhxFyLqnukm54360478 = -469790288;    float QMHCxuyOeYhxFyLqnukm5282170 = -630383610;    float QMHCxuyOeYhxFyLqnukm77997338 = -128360218;    float QMHCxuyOeYhxFyLqnukm68935005 = -685375583;    float QMHCxuyOeYhxFyLqnukm78187423 = -943589443;    float QMHCxuyOeYhxFyLqnukm36414519 = -114517;    float QMHCxuyOeYhxFyLqnukm34081701 = -455638354;    float QMHCxuyOeYhxFyLqnukm66506140 = -904876456;    float QMHCxuyOeYhxFyLqnukm26938878 = -26882159;    float QMHCxuyOeYhxFyLqnukm97130690 = -241487935;    float QMHCxuyOeYhxFyLqnukm99589813 = -847414353;    float QMHCxuyOeYhxFyLqnukm39522263 = -731683692;    float QMHCxuyOeYhxFyLqnukm60765834 = -823791691;    float QMHCxuyOeYhxFyLqnukm3644143 = -811430028;    float QMHCxuyOeYhxFyLqnukm57858361 = -109343873;    float QMHCxuyOeYhxFyLqnukm71554170 = -954895723;    float QMHCxuyOeYhxFyLqnukm62542767 = -487532517;    float QMHCxuyOeYhxFyLqnukm93428930 = -329750254;    float QMHCxuyOeYhxFyLqnukm96811026 = -366794286;    float QMHCxuyOeYhxFyLqnukm35830642 = -58885661;    float QMHCxuyOeYhxFyLqnukm42336774 = -491830754;    float QMHCxuyOeYhxFyLqnukm25883981 = -132048056;    float QMHCxuyOeYhxFyLqnukm51344483 = -4796500;    float QMHCxuyOeYhxFyLqnukm26884479 = -532595309;    float QMHCxuyOeYhxFyLqnukm97086929 = -580242558;    float QMHCxuyOeYhxFyLqnukm9702725 = -62197068;    float QMHCxuyOeYhxFyLqnukm85191800 = -553359607;    float QMHCxuyOeYhxFyLqnukm18568094 = -305984736;    float QMHCxuyOeYhxFyLqnukm81775319 = 64703343;    float QMHCxuyOeYhxFyLqnukm87961292 = -308279998;    float QMHCxuyOeYhxFyLqnukm27540265 = -421186382;    float QMHCxuyOeYhxFyLqnukm66614982 = -534658705;    float QMHCxuyOeYhxFyLqnukm45341568 = -149298131;    float QMHCxuyOeYhxFyLqnukm72180559 = -160939856;    float QMHCxuyOeYhxFyLqnukm44814652 = -928450851;    float QMHCxuyOeYhxFyLqnukm36818140 = -406940840;    float QMHCxuyOeYhxFyLqnukm18706109 = -253294157;    float QMHCxuyOeYhxFyLqnukm46893585 = -556442232;    float QMHCxuyOeYhxFyLqnukm99410665 = -794621281;    float QMHCxuyOeYhxFyLqnukm25621266 = -4337753;    float QMHCxuyOeYhxFyLqnukm94230305 = -619981295;    float QMHCxuyOeYhxFyLqnukm37224087 = -140569647;    float QMHCxuyOeYhxFyLqnukm11278907 = -705082134;    float QMHCxuyOeYhxFyLqnukm21799188 = -679277536;    float QMHCxuyOeYhxFyLqnukm91790557 = -972865946;    float QMHCxuyOeYhxFyLqnukm42381579 = -356631320;    float QMHCxuyOeYhxFyLqnukm22186766 = -432682565;    float QMHCxuyOeYhxFyLqnukm38069471 = -363069369;    float QMHCxuyOeYhxFyLqnukm49642441 = -450470483;    float QMHCxuyOeYhxFyLqnukm57976294 = -226720256;    float QMHCxuyOeYhxFyLqnukm6547862 = -646583791;    float QMHCxuyOeYhxFyLqnukm43505687 = 7176186;    float QMHCxuyOeYhxFyLqnukm79837355 = -837891191;    float QMHCxuyOeYhxFyLqnukm30065594 = -266817966;    float QMHCxuyOeYhxFyLqnukm74068329 = -861196256;    float QMHCxuyOeYhxFyLqnukm28476497 = -237742232;    float QMHCxuyOeYhxFyLqnukm53937687 = -525587111;    float QMHCxuyOeYhxFyLqnukm51112859 = -595764909;    float QMHCxuyOeYhxFyLqnukm71848076 = -5133025;    float QMHCxuyOeYhxFyLqnukm68484698 = -781392375;    float QMHCxuyOeYhxFyLqnukm51222719 = -446754910;    float QMHCxuyOeYhxFyLqnukm15513607 = -49653618;    float QMHCxuyOeYhxFyLqnukm84730820 = -571246555;     QMHCxuyOeYhxFyLqnukm58846811 = QMHCxuyOeYhxFyLqnukm64142648;     QMHCxuyOeYhxFyLqnukm64142648 = QMHCxuyOeYhxFyLqnukm46067001;     QMHCxuyOeYhxFyLqnukm46067001 = QMHCxuyOeYhxFyLqnukm64101765;     QMHCxuyOeYhxFyLqnukm64101765 = QMHCxuyOeYhxFyLqnukm70274361;     QMHCxuyOeYhxFyLqnukm70274361 = QMHCxuyOeYhxFyLqnukm18264851;     QMHCxuyOeYhxFyLqnukm18264851 = QMHCxuyOeYhxFyLqnukm78990771;     QMHCxuyOeYhxFyLqnukm78990771 = QMHCxuyOeYhxFyLqnukm92655324;     QMHCxuyOeYhxFyLqnukm92655324 = QMHCxuyOeYhxFyLqnukm26905413;     QMHCxuyOeYhxFyLqnukm26905413 = QMHCxuyOeYhxFyLqnukm26068896;     QMHCxuyOeYhxFyLqnukm26068896 = QMHCxuyOeYhxFyLqnukm11305784;     QMHCxuyOeYhxFyLqnukm11305784 = QMHCxuyOeYhxFyLqnukm57051921;     QMHCxuyOeYhxFyLqnukm57051921 = QMHCxuyOeYhxFyLqnukm64474881;     QMHCxuyOeYhxFyLqnukm64474881 = QMHCxuyOeYhxFyLqnukm43263364;     QMHCxuyOeYhxFyLqnukm43263364 = QMHCxuyOeYhxFyLqnukm11237804;     QMHCxuyOeYhxFyLqnukm11237804 = QMHCxuyOeYhxFyLqnukm88585661;     QMHCxuyOeYhxFyLqnukm88585661 = QMHCxuyOeYhxFyLqnukm99175129;     QMHCxuyOeYhxFyLqnukm99175129 = QMHCxuyOeYhxFyLqnukm42100310;     QMHCxuyOeYhxFyLqnukm42100310 = QMHCxuyOeYhxFyLqnukm96703446;     QMHCxuyOeYhxFyLqnukm96703446 = QMHCxuyOeYhxFyLqnukm15828591;     QMHCxuyOeYhxFyLqnukm15828591 = QMHCxuyOeYhxFyLqnukm77598088;     QMHCxuyOeYhxFyLqnukm77598088 = QMHCxuyOeYhxFyLqnukm62035785;     QMHCxuyOeYhxFyLqnukm62035785 = QMHCxuyOeYhxFyLqnukm28312006;     QMHCxuyOeYhxFyLqnukm28312006 = QMHCxuyOeYhxFyLqnukm3730227;     QMHCxuyOeYhxFyLqnukm3730227 = QMHCxuyOeYhxFyLqnukm38217784;     QMHCxuyOeYhxFyLqnukm38217784 = QMHCxuyOeYhxFyLqnukm18929879;     QMHCxuyOeYhxFyLqnukm18929879 = QMHCxuyOeYhxFyLqnukm91380371;     QMHCxuyOeYhxFyLqnukm91380371 = QMHCxuyOeYhxFyLqnukm81903842;     QMHCxuyOeYhxFyLqnukm81903842 = QMHCxuyOeYhxFyLqnukm82952600;     QMHCxuyOeYhxFyLqnukm82952600 = QMHCxuyOeYhxFyLqnukm41713613;     QMHCxuyOeYhxFyLqnukm41713613 = QMHCxuyOeYhxFyLqnukm7500802;     QMHCxuyOeYhxFyLqnukm7500802 = QMHCxuyOeYhxFyLqnukm29530464;     QMHCxuyOeYhxFyLqnukm29530464 = QMHCxuyOeYhxFyLqnukm69090629;     QMHCxuyOeYhxFyLqnukm69090629 = QMHCxuyOeYhxFyLqnukm36934617;     QMHCxuyOeYhxFyLqnukm36934617 = QMHCxuyOeYhxFyLqnukm76648382;     QMHCxuyOeYhxFyLqnukm76648382 = QMHCxuyOeYhxFyLqnukm65896236;     QMHCxuyOeYhxFyLqnukm65896236 = QMHCxuyOeYhxFyLqnukm16405103;     QMHCxuyOeYhxFyLqnukm16405103 = QMHCxuyOeYhxFyLqnukm54360478;     QMHCxuyOeYhxFyLqnukm54360478 = QMHCxuyOeYhxFyLqnukm5282170;     QMHCxuyOeYhxFyLqnukm5282170 = QMHCxuyOeYhxFyLqnukm77997338;     QMHCxuyOeYhxFyLqnukm77997338 = QMHCxuyOeYhxFyLqnukm68935005;     QMHCxuyOeYhxFyLqnukm68935005 = QMHCxuyOeYhxFyLqnukm78187423;     QMHCxuyOeYhxFyLqnukm78187423 = QMHCxuyOeYhxFyLqnukm36414519;     QMHCxuyOeYhxFyLqnukm36414519 = QMHCxuyOeYhxFyLqnukm34081701;     QMHCxuyOeYhxFyLqnukm34081701 = QMHCxuyOeYhxFyLqnukm66506140;     QMHCxuyOeYhxFyLqnukm66506140 = QMHCxuyOeYhxFyLqnukm26938878;     QMHCxuyOeYhxFyLqnukm26938878 = QMHCxuyOeYhxFyLqnukm97130690;     QMHCxuyOeYhxFyLqnukm97130690 = QMHCxuyOeYhxFyLqnukm99589813;     QMHCxuyOeYhxFyLqnukm99589813 = QMHCxuyOeYhxFyLqnukm39522263;     QMHCxuyOeYhxFyLqnukm39522263 = QMHCxuyOeYhxFyLqnukm60765834;     QMHCxuyOeYhxFyLqnukm60765834 = QMHCxuyOeYhxFyLqnukm3644143;     QMHCxuyOeYhxFyLqnukm3644143 = QMHCxuyOeYhxFyLqnukm57858361;     QMHCxuyOeYhxFyLqnukm57858361 = QMHCxuyOeYhxFyLqnukm71554170;     QMHCxuyOeYhxFyLqnukm71554170 = QMHCxuyOeYhxFyLqnukm62542767;     QMHCxuyOeYhxFyLqnukm62542767 = QMHCxuyOeYhxFyLqnukm93428930;     QMHCxuyOeYhxFyLqnukm93428930 = QMHCxuyOeYhxFyLqnukm96811026;     QMHCxuyOeYhxFyLqnukm96811026 = QMHCxuyOeYhxFyLqnukm35830642;     QMHCxuyOeYhxFyLqnukm35830642 = QMHCxuyOeYhxFyLqnukm42336774;     QMHCxuyOeYhxFyLqnukm42336774 = QMHCxuyOeYhxFyLqnukm25883981;     QMHCxuyOeYhxFyLqnukm25883981 = QMHCxuyOeYhxFyLqnukm51344483;     QMHCxuyOeYhxFyLqnukm51344483 = QMHCxuyOeYhxFyLqnukm26884479;     QMHCxuyOeYhxFyLqnukm26884479 = QMHCxuyOeYhxFyLqnukm97086929;     QMHCxuyOeYhxFyLqnukm97086929 = QMHCxuyOeYhxFyLqnukm9702725;     QMHCxuyOeYhxFyLqnukm9702725 = QMHCxuyOeYhxFyLqnukm85191800;     QMHCxuyOeYhxFyLqnukm85191800 = QMHCxuyOeYhxFyLqnukm18568094;     QMHCxuyOeYhxFyLqnukm18568094 = QMHCxuyOeYhxFyLqnukm81775319;     QMHCxuyOeYhxFyLqnukm81775319 = QMHCxuyOeYhxFyLqnukm87961292;     QMHCxuyOeYhxFyLqnukm87961292 = QMHCxuyOeYhxFyLqnukm27540265;     QMHCxuyOeYhxFyLqnukm27540265 = QMHCxuyOeYhxFyLqnukm66614982;     QMHCxuyOeYhxFyLqnukm66614982 = QMHCxuyOeYhxFyLqnukm45341568;     QMHCxuyOeYhxFyLqnukm45341568 = QMHCxuyOeYhxFyLqnukm72180559;     QMHCxuyOeYhxFyLqnukm72180559 = QMHCxuyOeYhxFyLqnukm44814652;     QMHCxuyOeYhxFyLqnukm44814652 = QMHCxuyOeYhxFyLqnukm36818140;     QMHCxuyOeYhxFyLqnukm36818140 = QMHCxuyOeYhxFyLqnukm18706109;     QMHCxuyOeYhxFyLqnukm18706109 = QMHCxuyOeYhxFyLqnukm46893585;     QMHCxuyOeYhxFyLqnukm46893585 = QMHCxuyOeYhxFyLqnukm99410665;     QMHCxuyOeYhxFyLqnukm99410665 = QMHCxuyOeYhxFyLqnukm25621266;     QMHCxuyOeYhxFyLqnukm25621266 = QMHCxuyOeYhxFyLqnukm94230305;     QMHCxuyOeYhxFyLqnukm94230305 = QMHCxuyOeYhxFyLqnukm37224087;     QMHCxuyOeYhxFyLqnukm37224087 = QMHCxuyOeYhxFyLqnukm11278907;     QMHCxuyOeYhxFyLqnukm11278907 = QMHCxuyOeYhxFyLqnukm21799188;     QMHCxuyOeYhxFyLqnukm21799188 = QMHCxuyOeYhxFyLqnukm91790557;     QMHCxuyOeYhxFyLqnukm91790557 = QMHCxuyOeYhxFyLqnukm42381579;     QMHCxuyOeYhxFyLqnukm42381579 = QMHCxuyOeYhxFyLqnukm22186766;     QMHCxuyOeYhxFyLqnukm22186766 = QMHCxuyOeYhxFyLqnukm38069471;     QMHCxuyOeYhxFyLqnukm38069471 = QMHCxuyOeYhxFyLqnukm49642441;     QMHCxuyOeYhxFyLqnukm49642441 = QMHCxuyOeYhxFyLqnukm57976294;     QMHCxuyOeYhxFyLqnukm57976294 = QMHCxuyOeYhxFyLqnukm6547862;     QMHCxuyOeYhxFyLqnukm6547862 = QMHCxuyOeYhxFyLqnukm43505687;     QMHCxuyOeYhxFyLqnukm43505687 = QMHCxuyOeYhxFyLqnukm79837355;     QMHCxuyOeYhxFyLqnukm79837355 = QMHCxuyOeYhxFyLqnukm30065594;     QMHCxuyOeYhxFyLqnukm30065594 = QMHCxuyOeYhxFyLqnukm74068329;     QMHCxuyOeYhxFyLqnukm74068329 = QMHCxuyOeYhxFyLqnukm28476497;     QMHCxuyOeYhxFyLqnukm28476497 = QMHCxuyOeYhxFyLqnukm53937687;     QMHCxuyOeYhxFyLqnukm53937687 = QMHCxuyOeYhxFyLqnukm51112859;     QMHCxuyOeYhxFyLqnukm51112859 = QMHCxuyOeYhxFyLqnukm71848076;     QMHCxuyOeYhxFyLqnukm71848076 = QMHCxuyOeYhxFyLqnukm68484698;     QMHCxuyOeYhxFyLqnukm68484698 = QMHCxuyOeYhxFyLqnukm51222719;     QMHCxuyOeYhxFyLqnukm51222719 = QMHCxuyOeYhxFyLqnukm15513607;     QMHCxuyOeYhxFyLqnukm15513607 = QMHCxuyOeYhxFyLqnukm84730820;     QMHCxuyOeYhxFyLqnukm84730820 = QMHCxuyOeYhxFyLqnukm58846811;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void fvrjGhYDlXeINvloiXdh21956590() {     float DFGLyhgQFgsCZlogsGta34433706 = -853148620;    float DFGLyhgQFgsCZlogsGta25896712 = -479334304;    float DFGLyhgQFgsCZlogsGta3617874 = -725456239;    float DFGLyhgQFgsCZlogsGta13553730 = -841200034;    float DFGLyhgQFgsCZlogsGta37406341 = -505395426;    float DFGLyhgQFgsCZlogsGta26572770 = -582998407;    float DFGLyhgQFgsCZlogsGta42174782 = -599694659;    float DFGLyhgQFgsCZlogsGta40361936 = -323308873;    float DFGLyhgQFgsCZlogsGta81549664 = -20671529;    float DFGLyhgQFgsCZlogsGta46067400 = -666158897;    float DFGLyhgQFgsCZlogsGta20559186 = 12527496;    float DFGLyhgQFgsCZlogsGta88796657 = -502235621;    float DFGLyhgQFgsCZlogsGta83092431 = -977594195;    float DFGLyhgQFgsCZlogsGta46362026 = -177580212;    float DFGLyhgQFgsCZlogsGta6213665 = -572869501;    float DFGLyhgQFgsCZlogsGta2665568 = -691959429;    float DFGLyhgQFgsCZlogsGta85010056 = -681657291;    float DFGLyhgQFgsCZlogsGta82466080 = -729333905;    float DFGLyhgQFgsCZlogsGta78061099 = -560132665;    float DFGLyhgQFgsCZlogsGta8971617 = -613251077;    float DFGLyhgQFgsCZlogsGta58689998 = -722293911;    float DFGLyhgQFgsCZlogsGta18964825 = -671687228;    float DFGLyhgQFgsCZlogsGta22856205 = -279972293;    float DFGLyhgQFgsCZlogsGta44531742 = -484064952;    float DFGLyhgQFgsCZlogsGta11010252 = -17786971;    float DFGLyhgQFgsCZlogsGta19275497 = -58164530;    float DFGLyhgQFgsCZlogsGta81212885 = -310893504;    float DFGLyhgQFgsCZlogsGta36951761 = -715735497;    float DFGLyhgQFgsCZlogsGta37660201 = -606250527;    float DFGLyhgQFgsCZlogsGta1013637 = -164551115;    float DFGLyhgQFgsCZlogsGta21194267 = -891688078;    float DFGLyhgQFgsCZlogsGta70539432 = -907456372;    float DFGLyhgQFgsCZlogsGta18914575 = -806451391;    float DFGLyhgQFgsCZlogsGta90450979 = -784548605;    float DFGLyhgQFgsCZlogsGta7463861 = -841292907;    float DFGLyhgQFgsCZlogsGta59981039 = -648356905;    float DFGLyhgQFgsCZlogsGta88065146 = 16374391;    float DFGLyhgQFgsCZlogsGta62453744 = -319689443;    float DFGLyhgQFgsCZlogsGta87377348 = -635551578;    float DFGLyhgQFgsCZlogsGta93812788 = -970972615;    float DFGLyhgQFgsCZlogsGta19122370 = -413449798;    float DFGLyhgQFgsCZlogsGta25772598 = -304179926;    float DFGLyhgQFgsCZlogsGta95543898 = -438938334;    float DFGLyhgQFgsCZlogsGta65067005 = 65187158;    float DFGLyhgQFgsCZlogsGta87482502 = 99417277;    float DFGLyhgQFgsCZlogsGta83232208 = -871418529;    float DFGLyhgQFgsCZlogsGta39974697 = -703636640;    float DFGLyhgQFgsCZlogsGta63856689 = -63406861;    float DFGLyhgQFgsCZlogsGta60668986 = -665418954;    float DFGLyhgQFgsCZlogsGta59564474 = -775058623;    float DFGLyhgQFgsCZlogsGta60465547 = -65449878;    float DFGLyhgQFgsCZlogsGta31480127 = 83113384;    float DFGLyhgQFgsCZlogsGta45765686 = -174780770;    float DFGLyhgQFgsCZlogsGta78879108 = -944616336;    float DFGLyhgQFgsCZlogsGta63266591 = -865615852;    float DFGLyhgQFgsCZlogsGta15468881 = -81461393;    float DFGLyhgQFgsCZlogsGta3040508 = -99362011;    float DFGLyhgQFgsCZlogsGta59086131 = -141391288;    float DFGLyhgQFgsCZlogsGta2543479 = -723413063;    float DFGLyhgQFgsCZlogsGta18130845 = -347230897;    float DFGLyhgQFgsCZlogsGta45359884 = -172104903;    float DFGLyhgQFgsCZlogsGta5223022 = -883959163;    float DFGLyhgQFgsCZlogsGta2701736 = -717058346;    float DFGLyhgQFgsCZlogsGta80536028 = -856120415;    float DFGLyhgQFgsCZlogsGta24873134 = -774470819;    float DFGLyhgQFgsCZlogsGta50019754 = -80016132;    float DFGLyhgQFgsCZlogsGta69882082 = -695784231;    float DFGLyhgQFgsCZlogsGta92641451 = -93045590;    float DFGLyhgQFgsCZlogsGta38898166 = -336287305;    float DFGLyhgQFgsCZlogsGta46232625 = -924512596;    float DFGLyhgQFgsCZlogsGta14600422 = -608333820;    float DFGLyhgQFgsCZlogsGta22556312 = -261967848;    float DFGLyhgQFgsCZlogsGta95088731 = 6217673;    float DFGLyhgQFgsCZlogsGta84248311 = -589160050;    float DFGLyhgQFgsCZlogsGta89849247 = -99801279;    float DFGLyhgQFgsCZlogsGta32917400 = -318113985;    float DFGLyhgQFgsCZlogsGta23420927 = -132748894;    float DFGLyhgQFgsCZlogsGta57789200 = -245159452;    float DFGLyhgQFgsCZlogsGta57049239 = -483482229;    float DFGLyhgQFgsCZlogsGta27778043 = -146368443;    float DFGLyhgQFgsCZlogsGta79300799 = -354527890;    float DFGLyhgQFgsCZlogsGta17356196 = -147486644;    float DFGLyhgQFgsCZlogsGta76282775 = 49683456;    float DFGLyhgQFgsCZlogsGta78095727 = -831191905;    float DFGLyhgQFgsCZlogsGta40548089 = 898763;    float DFGLyhgQFgsCZlogsGta89714140 = -874801463;    float DFGLyhgQFgsCZlogsGta24773746 = -632675603;    float DFGLyhgQFgsCZlogsGta40035467 = -861835055;    float DFGLyhgQFgsCZlogsGta27184388 = -918932754;    float DFGLyhgQFgsCZlogsGta91994979 = -659831515;    float DFGLyhgQFgsCZlogsGta56940532 = -448994894;    float DFGLyhgQFgsCZlogsGta28979015 = -842234322;    float DFGLyhgQFgsCZlogsGta59910266 = -596276381;    float DFGLyhgQFgsCZlogsGta69246504 = -188320682;    float DFGLyhgQFgsCZlogsGta48452904 = -698867713;    float DFGLyhgQFgsCZlogsGta13899349 = -529490636;    float DFGLyhgQFgsCZlogsGta23070862 = -587121581;    float DFGLyhgQFgsCZlogsGta15007870 = -582817920;    float DFGLyhgQFgsCZlogsGta40193872 = -160342023;    float DFGLyhgQFgsCZlogsGta37462749 = -853148620;     DFGLyhgQFgsCZlogsGta34433706 = DFGLyhgQFgsCZlogsGta25896712;     DFGLyhgQFgsCZlogsGta25896712 = DFGLyhgQFgsCZlogsGta3617874;     DFGLyhgQFgsCZlogsGta3617874 = DFGLyhgQFgsCZlogsGta13553730;     DFGLyhgQFgsCZlogsGta13553730 = DFGLyhgQFgsCZlogsGta37406341;     DFGLyhgQFgsCZlogsGta37406341 = DFGLyhgQFgsCZlogsGta26572770;     DFGLyhgQFgsCZlogsGta26572770 = DFGLyhgQFgsCZlogsGta42174782;     DFGLyhgQFgsCZlogsGta42174782 = DFGLyhgQFgsCZlogsGta40361936;     DFGLyhgQFgsCZlogsGta40361936 = DFGLyhgQFgsCZlogsGta81549664;     DFGLyhgQFgsCZlogsGta81549664 = DFGLyhgQFgsCZlogsGta46067400;     DFGLyhgQFgsCZlogsGta46067400 = DFGLyhgQFgsCZlogsGta20559186;     DFGLyhgQFgsCZlogsGta20559186 = DFGLyhgQFgsCZlogsGta88796657;     DFGLyhgQFgsCZlogsGta88796657 = DFGLyhgQFgsCZlogsGta83092431;     DFGLyhgQFgsCZlogsGta83092431 = DFGLyhgQFgsCZlogsGta46362026;     DFGLyhgQFgsCZlogsGta46362026 = DFGLyhgQFgsCZlogsGta6213665;     DFGLyhgQFgsCZlogsGta6213665 = DFGLyhgQFgsCZlogsGta2665568;     DFGLyhgQFgsCZlogsGta2665568 = DFGLyhgQFgsCZlogsGta85010056;     DFGLyhgQFgsCZlogsGta85010056 = DFGLyhgQFgsCZlogsGta82466080;     DFGLyhgQFgsCZlogsGta82466080 = DFGLyhgQFgsCZlogsGta78061099;     DFGLyhgQFgsCZlogsGta78061099 = DFGLyhgQFgsCZlogsGta8971617;     DFGLyhgQFgsCZlogsGta8971617 = DFGLyhgQFgsCZlogsGta58689998;     DFGLyhgQFgsCZlogsGta58689998 = DFGLyhgQFgsCZlogsGta18964825;     DFGLyhgQFgsCZlogsGta18964825 = DFGLyhgQFgsCZlogsGta22856205;     DFGLyhgQFgsCZlogsGta22856205 = DFGLyhgQFgsCZlogsGta44531742;     DFGLyhgQFgsCZlogsGta44531742 = DFGLyhgQFgsCZlogsGta11010252;     DFGLyhgQFgsCZlogsGta11010252 = DFGLyhgQFgsCZlogsGta19275497;     DFGLyhgQFgsCZlogsGta19275497 = DFGLyhgQFgsCZlogsGta81212885;     DFGLyhgQFgsCZlogsGta81212885 = DFGLyhgQFgsCZlogsGta36951761;     DFGLyhgQFgsCZlogsGta36951761 = DFGLyhgQFgsCZlogsGta37660201;     DFGLyhgQFgsCZlogsGta37660201 = DFGLyhgQFgsCZlogsGta1013637;     DFGLyhgQFgsCZlogsGta1013637 = DFGLyhgQFgsCZlogsGta21194267;     DFGLyhgQFgsCZlogsGta21194267 = DFGLyhgQFgsCZlogsGta70539432;     DFGLyhgQFgsCZlogsGta70539432 = DFGLyhgQFgsCZlogsGta18914575;     DFGLyhgQFgsCZlogsGta18914575 = DFGLyhgQFgsCZlogsGta90450979;     DFGLyhgQFgsCZlogsGta90450979 = DFGLyhgQFgsCZlogsGta7463861;     DFGLyhgQFgsCZlogsGta7463861 = DFGLyhgQFgsCZlogsGta59981039;     DFGLyhgQFgsCZlogsGta59981039 = DFGLyhgQFgsCZlogsGta88065146;     DFGLyhgQFgsCZlogsGta88065146 = DFGLyhgQFgsCZlogsGta62453744;     DFGLyhgQFgsCZlogsGta62453744 = DFGLyhgQFgsCZlogsGta87377348;     DFGLyhgQFgsCZlogsGta87377348 = DFGLyhgQFgsCZlogsGta93812788;     DFGLyhgQFgsCZlogsGta93812788 = DFGLyhgQFgsCZlogsGta19122370;     DFGLyhgQFgsCZlogsGta19122370 = DFGLyhgQFgsCZlogsGta25772598;     DFGLyhgQFgsCZlogsGta25772598 = DFGLyhgQFgsCZlogsGta95543898;     DFGLyhgQFgsCZlogsGta95543898 = DFGLyhgQFgsCZlogsGta65067005;     DFGLyhgQFgsCZlogsGta65067005 = DFGLyhgQFgsCZlogsGta87482502;     DFGLyhgQFgsCZlogsGta87482502 = DFGLyhgQFgsCZlogsGta83232208;     DFGLyhgQFgsCZlogsGta83232208 = DFGLyhgQFgsCZlogsGta39974697;     DFGLyhgQFgsCZlogsGta39974697 = DFGLyhgQFgsCZlogsGta63856689;     DFGLyhgQFgsCZlogsGta63856689 = DFGLyhgQFgsCZlogsGta60668986;     DFGLyhgQFgsCZlogsGta60668986 = DFGLyhgQFgsCZlogsGta59564474;     DFGLyhgQFgsCZlogsGta59564474 = DFGLyhgQFgsCZlogsGta60465547;     DFGLyhgQFgsCZlogsGta60465547 = DFGLyhgQFgsCZlogsGta31480127;     DFGLyhgQFgsCZlogsGta31480127 = DFGLyhgQFgsCZlogsGta45765686;     DFGLyhgQFgsCZlogsGta45765686 = DFGLyhgQFgsCZlogsGta78879108;     DFGLyhgQFgsCZlogsGta78879108 = DFGLyhgQFgsCZlogsGta63266591;     DFGLyhgQFgsCZlogsGta63266591 = DFGLyhgQFgsCZlogsGta15468881;     DFGLyhgQFgsCZlogsGta15468881 = DFGLyhgQFgsCZlogsGta3040508;     DFGLyhgQFgsCZlogsGta3040508 = DFGLyhgQFgsCZlogsGta59086131;     DFGLyhgQFgsCZlogsGta59086131 = DFGLyhgQFgsCZlogsGta2543479;     DFGLyhgQFgsCZlogsGta2543479 = DFGLyhgQFgsCZlogsGta18130845;     DFGLyhgQFgsCZlogsGta18130845 = DFGLyhgQFgsCZlogsGta45359884;     DFGLyhgQFgsCZlogsGta45359884 = DFGLyhgQFgsCZlogsGta5223022;     DFGLyhgQFgsCZlogsGta5223022 = DFGLyhgQFgsCZlogsGta2701736;     DFGLyhgQFgsCZlogsGta2701736 = DFGLyhgQFgsCZlogsGta80536028;     DFGLyhgQFgsCZlogsGta80536028 = DFGLyhgQFgsCZlogsGta24873134;     DFGLyhgQFgsCZlogsGta24873134 = DFGLyhgQFgsCZlogsGta50019754;     DFGLyhgQFgsCZlogsGta50019754 = DFGLyhgQFgsCZlogsGta69882082;     DFGLyhgQFgsCZlogsGta69882082 = DFGLyhgQFgsCZlogsGta92641451;     DFGLyhgQFgsCZlogsGta92641451 = DFGLyhgQFgsCZlogsGta38898166;     DFGLyhgQFgsCZlogsGta38898166 = DFGLyhgQFgsCZlogsGta46232625;     DFGLyhgQFgsCZlogsGta46232625 = DFGLyhgQFgsCZlogsGta14600422;     DFGLyhgQFgsCZlogsGta14600422 = DFGLyhgQFgsCZlogsGta22556312;     DFGLyhgQFgsCZlogsGta22556312 = DFGLyhgQFgsCZlogsGta95088731;     DFGLyhgQFgsCZlogsGta95088731 = DFGLyhgQFgsCZlogsGta84248311;     DFGLyhgQFgsCZlogsGta84248311 = DFGLyhgQFgsCZlogsGta89849247;     DFGLyhgQFgsCZlogsGta89849247 = DFGLyhgQFgsCZlogsGta32917400;     DFGLyhgQFgsCZlogsGta32917400 = DFGLyhgQFgsCZlogsGta23420927;     DFGLyhgQFgsCZlogsGta23420927 = DFGLyhgQFgsCZlogsGta57789200;     DFGLyhgQFgsCZlogsGta57789200 = DFGLyhgQFgsCZlogsGta57049239;     DFGLyhgQFgsCZlogsGta57049239 = DFGLyhgQFgsCZlogsGta27778043;     DFGLyhgQFgsCZlogsGta27778043 = DFGLyhgQFgsCZlogsGta79300799;     DFGLyhgQFgsCZlogsGta79300799 = DFGLyhgQFgsCZlogsGta17356196;     DFGLyhgQFgsCZlogsGta17356196 = DFGLyhgQFgsCZlogsGta76282775;     DFGLyhgQFgsCZlogsGta76282775 = DFGLyhgQFgsCZlogsGta78095727;     DFGLyhgQFgsCZlogsGta78095727 = DFGLyhgQFgsCZlogsGta40548089;     DFGLyhgQFgsCZlogsGta40548089 = DFGLyhgQFgsCZlogsGta89714140;     DFGLyhgQFgsCZlogsGta89714140 = DFGLyhgQFgsCZlogsGta24773746;     DFGLyhgQFgsCZlogsGta24773746 = DFGLyhgQFgsCZlogsGta40035467;     DFGLyhgQFgsCZlogsGta40035467 = DFGLyhgQFgsCZlogsGta27184388;     DFGLyhgQFgsCZlogsGta27184388 = DFGLyhgQFgsCZlogsGta91994979;     DFGLyhgQFgsCZlogsGta91994979 = DFGLyhgQFgsCZlogsGta56940532;     DFGLyhgQFgsCZlogsGta56940532 = DFGLyhgQFgsCZlogsGta28979015;     DFGLyhgQFgsCZlogsGta28979015 = DFGLyhgQFgsCZlogsGta59910266;     DFGLyhgQFgsCZlogsGta59910266 = DFGLyhgQFgsCZlogsGta69246504;     DFGLyhgQFgsCZlogsGta69246504 = DFGLyhgQFgsCZlogsGta48452904;     DFGLyhgQFgsCZlogsGta48452904 = DFGLyhgQFgsCZlogsGta13899349;     DFGLyhgQFgsCZlogsGta13899349 = DFGLyhgQFgsCZlogsGta23070862;     DFGLyhgQFgsCZlogsGta23070862 = DFGLyhgQFgsCZlogsGta15007870;     DFGLyhgQFgsCZlogsGta15007870 = DFGLyhgQFgsCZlogsGta40193872;     DFGLyhgQFgsCZlogsGta40193872 = DFGLyhgQFgsCZlogsGta37462749;     DFGLyhgQFgsCZlogsGta37462749 = DFGLyhgQFgsCZlogsGta34433706;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void SlNOCVKsmzhLVpKwHlPs28615334() {     float WemsgUvKTMZZUePojfZm34427235 = -725672762;    float WemsgUvKTMZZUePojfZm64469028 = -521929429;    float WemsgUvKTMZZUePojfZm4931086 = -682584852;    float WemsgUvKTMZZUePojfZm98633590 = -661740864;    float WemsgUvKTMZZUePojfZm46246443 = -973750748;    float WemsgUvKTMZZUePojfZm61378202 = -282431219;    float WemsgUvKTMZZUePojfZm10722118 = -304231126;    float WemsgUvKTMZZUePojfZm9575987 = -27486190;    float WemsgUvKTMZZUePojfZm64289829 = -834317599;    float WemsgUvKTMZZUePojfZm42706914 = -302965768;    float WemsgUvKTMZZUePojfZm40879791 = 53055424;    float WemsgUvKTMZZUePojfZm3088335 = -237713984;    float WemsgUvKTMZZUePojfZm14945345 = -999252669;    float WemsgUvKTMZZUePojfZm206247 = -522733264;    float WemsgUvKTMZZUePojfZm51385773 = -535582353;    float WemsgUvKTMZZUePojfZm77230114 = 75244920;    float WemsgUvKTMZZUePojfZm40862815 = -501271860;    float WemsgUvKTMZZUePojfZm5466614 = -26405527;    float WemsgUvKTMZZUePojfZm98489369 = -78114931;    float WemsgUvKTMZZUePojfZm81045379 = -125275528;    float WemsgUvKTMZZUePojfZm38359422 = -432760284;    float WemsgUvKTMZZUePojfZm97261090 = -81679111;    float WemsgUvKTMZZUePojfZm59734602 = -738930135;    float WemsgUvKTMZZUePojfZm88181850 = -364985474;    float WemsgUvKTMZZUePojfZm39452941 = -444480825;    float WemsgUvKTMZZUePojfZm35849573 = 33629542;    float WemsgUvKTMZZUePojfZm51434467 = -611693217;    float WemsgUvKTMZZUePojfZm72355286 = -224702486;    float WemsgUvKTMZZUePojfZm50338648 = -152320842;    float WemsgUvKTMZZUePojfZm77103506 = -939982488;    float WemsgUvKTMZZUePojfZm54606945 = -178781313;    float WemsgUvKTMZZUePojfZm55263289 = -199278134;    float WemsgUvKTMZZUePojfZm4366999 = 91604393;    float WemsgUvKTMZZUePojfZm90733816 = -661230338;    float WemsgUvKTMZZUePojfZm89821898 = -606832;    float WemsgUvKTMZZUePojfZm2809229 = -728122651;    float WemsgUvKTMZZUePojfZm6514 = -83278987;    float WemsgUvKTMZZUePojfZm32371424 = -314952122;    float WemsgUvKTMZZUePojfZm22690946 = -248232994;    float WemsgUvKTMZZUePojfZm65685681 = -580404301;    float WemsgUvKTMZZUePojfZm6978038 = -501393696;    float WemsgUvKTMZZUePojfZm11470259 = -497972008;    float WemsgUvKTMZZUePojfZm55827258 = -998068589;    float WemsgUvKTMZZUePojfZm24298904 = -591777285;    float WemsgUvKTMZZUePojfZm71500555 = -373842125;    float WemsgUvKTMZZUePojfZm23660177 = -683463200;    float WemsgUvKTMZZUePojfZm67672489 = -84917262;    float WemsgUvKTMZZUePojfZm39240271 = -672168217;    float WemsgUvKTMZZUePojfZm79327375 = -229182815;    float WemsgUvKTMZZUePojfZm79609896 = -362083509;    float WemsgUvKTMZZUePojfZm73724917 = -744004885;    float WemsgUvKTMZZUePojfZm46027384 = -299032364;    float WemsgUvKTMZZUePojfZm21214035 = -66834778;    float WemsgUvKTMZZUePojfZm89129379 = -376536187;    float WemsgUvKTMZZUePojfZm90255912 = -622515129;    float WemsgUvKTMZZUePojfZm37166144 = -543993651;    float WemsgUvKTMZZUePojfZm4734427 = -782999295;    float WemsgUvKTMZZUePojfZm16749235 = -217599379;    float WemsgUvKTMZZUePojfZm59180649 = -117260040;    float WemsgUvKTMZZUePojfZm10396870 = -907380290;    float WemsgUvKTMZZUePojfZm9943735 = -670738002;    float WemsgUvKTMZZUePojfZm38366832 = 20471359;    float WemsgUvKTMZZUePojfZm59237339 = -875165348;    float WemsgUvKTMZZUePojfZm87186322 = -894335112;    float WemsgUvKTMZZUePojfZm88099968 = -24184456;    float WemsgUvKTMZZUePojfZm85616501 = -747666443;    float WemsgUvKTMZZUePojfZm98721336 = -229318378;    float WemsgUvKTMZZUePojfZm24211529 = -238022332;    float WemsgUvKTMZZUePojfZm10384348 = -422126432;    float WemsgUvKTMZZUePojfZm48576545 = -807459702;    float WemsgUvKTMZZUePojfZm77223601 = -841476093;    float WemsgUvKTMZZUePojfZm8491392 = -86319738;    float WemsgUvKTMZZUePojfZm82775668 = -778172533;    float WemsgUvKTMZZUePojfZm32803688 = -497710631;    float WemsgUvKTMZZUePojfZm74067342 = -623881832;    float WemsgUvKTMZZUePojfZm26889164 = -934788276;    float WemsgUvKTMZZUePojfZm41433833 = -83610523;    float WemsgUvKTMZZUePojfZm35435698 = -47152850;    float WemsgUvKTMZZUePojfZm16681296 = -991143350;    float WemsgUvKTMZZUePojfZm15792765 = -761017626;    float WemsgUvKTMZZUePojfZm68177083 = -881453196;    float WemsgUvKTMZZUePojfZm12194197 = -939525001;    float WemsgUvKTMZZUePojfZm93027910 = -995519671;    float WemsgUvKTMZZUePojfZm70728751 = -790237333;    float WemsgUvKTMZZUePojfZm3378590 = -95977603;    float WemsgUvKTMZZUePojfZm8579562 = -879748949;    float WemsgUvKTMZZUePojfZm34049255 = -32443356;    float WemsgUvKTMZZUePojfZm15237620 = -531859420;    float WemsgUvKTMZZUePojfZm477905 = 61284791;    float WemsgUvKTMZZUePojfZm52655755 = -456613181;    float WemsgUvKTMZZUePojfZm98074801 = -945123357;    float WemsgUvKTMZZUePojfZm83257279 = -865679609;    float WemsgUvKTMZZUePojfZm73190774 = -97692083;    float WemsgUvKTMZZUePojfZm12294076 = -340852705;    float WemsgUvKTMZZUePojfZm55741947 = -909666300;    float WemsgUvKTMZZUePojfZm68611205 = -421865056;    float WemsgUvKTMZZUePojfZm52232919 = -622806661;    float WemsgUvKTMZZUePojfZm68640935 = -3733478;    float WemsgUvKTMZZUePojfZm36198935 = -467592830;    float WemsgUvKTMZZUePojfZm85884053 = -725672762;     WemsgUvKTMZZUePojfZm34427235 = WemsgUvKTMZZUePojfZm64469028;     WemsgUvKTMZZUePojfZm64469028 = WemsgUvKTMZZUePojfZm4931086;     WemsgUvKTMZZUePojfZm4931086 = WemsgUvKTMZZUePojfZm98633590;     WemsgUvKTMZZUePojfZm98633590 = WemsgUvKTMZZUePojfZm46246443;     WemsgUvKTMZZUePojfZm46246443 = WemsgUvKTMZZUePojfZm61378202;     WemsgUvKTMZZUePojfZm61378202 = WemsgUvKTMZZUePojfZm10722118;     WemsgUvKTMZZUePojfZm10722118 = WemsgUvKTMZZUePojfZm9575987;     WemsgUvKTMZZUePojfZm9575987 = WemsgUvKTMZZUePojfZm64289829;     WemsgUvKTMZZUePojfZm64289829 = WemsgUvKTMZZUePojfZm42706914;     WemsgUvKTMZZUePojfZm42706914 = WemsgUvKTMZZUePojfZm40879791;     WemsgUvKTMZZUePojfZm40879791 = WemsgUvKTMZZUePojfZm3088335;     WemsgUvKTMZZUePojfZm3088335 = WemsgUvKTMZZUePojfZm14945345;     WemsgUvKTMZZUePojfZm14945345 = WemsgUvKTMZZUePojfZm206247;     WemsgUvKTMZZUePojfZm206247 = WemsgUvKTMZZUePojfZm51385773;     WemsgUvKTMZZUePojfZm51385773 = WemsgUvKTMZZUePojfZm77230114;     WemsgUvKTMZZUePojfZm77230114 = WemsgUvKTMZZUePojfZm40862815;     WemsgUvKTMZZUePojfZm40862815 = WemsgUvKTMZZUePojfZm5466614;     WemsgUvKTMZZUePojfZm5466614 = WemsgUvKTMZZUePojfZm98489369;     WemsgUvKTMZZUePojfZm98489369 = WemsgUvKTMZZUePojfZm81045379;     WemsgUvKTMZZUePojfZm81045379 = WemsgUvKTMZZUePojfZm38359422;     WemsgUvKTMZZUePojfZm38359422 = WemsgUvKTMZZUePojfZm97261090;     WemsgUvKTMZZUePojfZm97261090 = WemsgUvKTMZZUePojfZm59734602;     WemsgUvKTMZZUePojfZm59734602 = WemsgUvKTMZZUePojfZm88181850;     WemsgUvKTMZZUePojfZm88181850 = WemsgUvKTMZZUePojfZm39452941;     WemsgUvKTMZZUePojfZm39452941 = WemsgUvKTMZZUePojfZm35849573;     WemsgUvKTMZZUePojfZm35849573 = WemsgUvKTMZZUePojfZm51434467;     WemsgUvKTMZZUePojfZm51434467 = WemsgUvKTMZZUePojfZm72355286;     WemsgUvKTMZZUePojfZm72355286 = WemsgUvKTMZZUePojfZm50338648;     WemsgUvKTMZZUePojfZm50338648 = WemsgUvKTMZZUePojfZm77103506;     WemsgUvKTMZZUePojfZm77103506 = WemsgUvKTMZZUePojfZm54606945;     WemsgUvKTMZZUePojfZm54606945 = WemsgUvKTMZZUePojfZm55263289;     WemsgUvKTMZZUePojfZm55263289 = WemsgUvKTMZZUePojfZm4366999;     WemsgUvKTMZZUePojfZm4366999 = WemsgUvKTMZZUePojfZm90733816;     WemsgUvKTMZZUePojfZm90733816 = WemsgUvKTMZZUePojfZm89821898;     WemsgUvKTMZZUePojfZm89821898 = WemsgUvKTMZZUePojfZm2809229;     WemsgUvKTMZZUePojfZm2809229 = WemsgUvKTMZZUePojfZm6514;     WemsgUvKTMZZUePojfZm6514 = WemsgUvKTMZZUePojfZm32371424;     WemsgUvKTMZZUePojfZm32371424 = WemsgUvKTMZZUePojfZm22690946;     WemsgUvKTMZZUePojfZm22690946 = WemsgUvKTMZZUePojfZm65685681;     WemsgUvKTMZZUePojfZm65685681 = WemsgUvKTMZZUePojfZm6978038;     WemsgUvKTMZZUePojfZm6978038 = WemsgUvKTMZZUePojfZm11470259;     WemsgUvKTMZZUePojfZm11470259 = WemsgUvKTMZZUePojfZm55827258;     WemsgUvKTMZZUePojfZm55827258 = WemsgUvKTMZZUePojfZm24298904;     WemsgUvKTMZZUePojfZm24298904 = WemsgUvKTMZZUePojfZm71500555;     WemsgUvKTMZZUePojfZm71500555 = WemsgUvKTMZZUePojfZm23660177;     WemsgUvKTMZZUePojfZm23660177 = WemsgUvKTMZZUePojfZm67672489;     WemsgUvKTMZZUePojfZm67672489 = WemsgUvKTMZZUePojfZm39240271;     WemsgUvKTMZZUePojfZm39240271 = WemsgUvKTMZZUePojfZm79327375;     WemsgUvKTMZZUePojfZm79327375 = WemsgUvKTMZZUePojfZm79609896;     WemsgUvKTMZZUePojfZm79609896 = WemsgUvKTMZZUePojfZm73724917;     WemsgUvKTMZZUePojfZm73724917 = WemsgUvKTMZZUePojfZm46027384;     WemsgUvKTMZZUePojfZm46027384 = WemsgUvKTMZZUePojfZm21214035;     WemsgUvKTMZZUePojfZm21214035 = WemsgUvKTMZZUePojfZm89129379;     WemsgUvKTMZZUePojfZm89129379 = WemsgUvKTMZZUePojfZm90255912;     WemsgUvKTMZZUePojfZm90255912 = WemsgUvKTMZZUePojfZm37166144;     WemsgUvKTMZZUePojfZm37166144 = WemsgUvKTMZZUePojfZm4734427;     WemsgUvKTMZZUePojfZm4734427 = WemsgUvKTMZZUePojfZm16749235;     WemsgUvKTMZZUePojfZm16749235 = WemsgUvKTMZZUePojfZm59180649;     WemsgUvKTMZZUePojfZm59180649 = WemsgUvKTMZZUePojfZm10396870;     WemsgUvKTMZZUePojfZm10396870 = WemsgUvKTMZZUePojfZm9943735;     WemsgUvKTMZZUePojfZm9943735 = WemsgUvKTMZZUePojfZm38366832;     WemsgUvKTMZZUePojfZm38366832 = WemsgUvKTMZZUePojfZm59237339;     WemsgUvKTMZZUePojfZm59237339 = WemsgUvKTMZZUePojfZm87186322;     WemsgUvKTMZZUePojfZm87186322 = WemsgUvKTMZZUePojfZm88099968;     WemsgUvKTMZZUePojfZm88099968 = WemsgUvKTMZZUePojfZm85616501;     WemsgUvKTMZZUePojfZm85616501 = WemsgUvKTMZZUePojfZm98721336;     WemsgUvKTMZZUePojfZm98721336 = WemsgUvKTMZZUePojfZm24211529;     WemsgUvKTMZZUePojfZm24211529 = WemsgUvKTMZZUePojfZm10384348;     WemsgUvKTMZZUePojfZm10384348 = WemsgUvKTMZZUePojfZm48576545;     WemsgUvKTMZZUePojfZm48576545 = WemsgUvKTMZZUePojfZm77223601;     WemsgUvKTMZZUePojfZm77223601 = WemsgUvKTMZZUePojfZm8491392;     WemsgUvKTMZZUePojfZm8491392 = WemsgUvKTMZZUePojfZm82775668;     WemsgUvKTMZZUePojfZm82775668 = WemsgUvKTMZZUePojfZm32803688;     WemsgUvKTMZZUePojfZm32803688 = WemsgUvKTMZZUePojfZm74067342;     WemsgUvKTMZZUePojfZm74067342 = WemsgUvKTMZZUePojfZm26889164;     WemsgUvKTMZZUePojfZm26889164 = WemsgUvKTMZZUePojfZm41433833;     WemsgUvKTMZZUePojfZm41433833 = WemsgUvKTMZZUePojfZm35435698;     WemsgUvKTMZZUePojfZm35435698 = WemsgUvKTMZZUePojfZm16681296;     WemsgUvKTMZZUePojfZm16681296 = WemsgUvKTMZZUePojfZm15792765;     WemsgUvKTMZZUePojfZm15792765 = WemsgUvKTMZZUePojfZm68177083;     WemsgUvKTMZZUePojfZm68177083 = WemsgUvKTMZZUePojfZm12194197;     WemsgUvKTMZZUePojfZm12194197 = WemsgUvKTMZZUePojfZm93027910;     WemsgUvKTMZZUePojfZm93027910 = WemsgUvKTMZZUePojfZm70728751;     WemsgUvKTMZZUePojfZm70728751 = WemsgUvKTMZZUePojfZm3378590;     WemsgUvKTMZZUePojfZm3378590 = WemsgUvKTMZZUePojfZm8579562;     WemsgUvKTMZZUePojfZm8579562 = WemsgUvKTMZZUePojfZm34049255;     WemsgUvKTMZZUePojfZm34049255 = WemsgUvKTMZZUePojfZm15237620;     WemsgUvKTMZZUePojfZm15237620 = WemsgUvKTMZZUePojfZm477905;     WemsgUvKTMZZUePojfZm477905 = WemsgUvKTMZZUePojfZm52655755;     WemsgUvKTMZZUePojfZm52655755 = WemsgUvKTMZZUePojfZm98074801;     WemsgUvKTMZZUePojfZm98074801 = WemsgUvKTMZZUePojfZm83257279;     WemsgUvKTMZZUePojfZm83257279 = WemsgUvKTMZZUePojfZm73190774;     WemsgUvKTMZZUePojfZm73190774 = WemsgUvKTMZZUePojfZm12294076;     WemsgUvKTMZZUePojfZm12294076 = WemsgUvKTMZZUePojfZm55741947;     WemsgUvKTMZZUePojfZm55741947 = WemsgUvKTMZZUePojfZm68611205;     WemsgUvKTMZZUePojfZm68611205 = WemsgUvKTMZZUePojfZm52232919;     WemsgUvKTMZZUePojfZm52232919 = WemsgUvKTMZZUePojfZm68640935;     WemsgUvKTMZZUePojfZm68640935 = WemsgUvKTMZZUePojfZm36198935;     WemsgUvKTMZZUePojfZm36198935 = WemsgUvKTMZZUePojfZm85884053;     WemsgUvKTMZZUePojfZm85884053 = WemsgUvKTMZZUePojfZm34427235;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void AjOuStaodJuXlJkTxvga83031547() {     float OhLTzvsPgtuSgEmyggHF5078902 = -243821892;    float OhLTzvsPgtuSgEmyggHF33142546 = -393572728;    float OhLTzvsPgtuSgEmyggHF84900004 = -24879312;    float OhLTzvsPgtuSgEmyggHF38564877 = -851879253;    float OhLTzvsPgtuSgEmyggHF67696764 = -757157095;    float OhLTzvsPgtuSgEmyggHF51377964 = 15987630;    float OhLTzvsPgtuSgEmyggHF29101014 = -303006836;    float OhLTzvsPgtuSgEmyggHF66418367 = -374303280;    float OhLTzvsPgtuSgEmyggHF10154884 = -927664928;    float OhLTzvsPgtuSgEmyggHF65673374 = -604061118;    float OhLTzvsPgtuSgEmyggHF83128952 = 18153495;    float OhLTzvsPgtuSgEmyggHF48634906 = -553477777;    float OhLTzvsPgtuSgEmyggHF94772731 = -908789728;    float OhLTzvsPgtuSgEmyggHF84053280 = -323761474;    float OhLTzvsPgtuSgEmyggHF34105793 = -685332356;    float OhLTzvsPgtuSgEmyggHF9758755 = -594738667;    float OhLTzvsPgtuSgEmyggHF34715642 = -410456990;    float OhLTzvsPgtuSgEmyggHF25760108 = -15571723;    float OhLTzvsPgtuSgEmyggHF80197151 = -785302873;    float OhLTzvsPgtuSgEmyggHF39832089 = -667641833;    float OhLTzvsPgtuSgEmyggHF93495176 = -341439425;    float OhLTzvsPgtuSgEmyggHF58656856 = -412964417;    float OhLTzvsPgtuSgEmyggHF61678410 = -206773280;    float OhLTzvsPgtuSgEmyggHF22031388 = 42613620;    float OhLTzvsPgtuSgEmyggHF3577330 = -120385512;    float OhLTzvsPgtuSgEmyggHF66447154 = -64654903;    float OhLTzvsPgtuSgEmyggHF59695253 = 93129008;    float OhLTzvsPgtuSgEmyggHF80137511 = -729826253;    float OhLTzvsPgtuSgEmyggHF82481135 = -446679059;    float OhLTzvsPgtuSgEmyggHF16275602 = -398377559;    float OhLTzvsPgtuSgEmyggHF61678942 = 72785002;    float OhLTzvsPgtuSgEmyggHF99774750 = -527575297;    float OhLTzvsPgtuSgEmyggHF87312359 = -955357215;    float OhLTzvsPgtuSgEmyggHF56098791 = -224760022;    float OhLTzvsPgtuSgEmyggHF67846135 = -947553612;    float OhLTzvsPgtuSgEmyggHF62802487 = -807349199;    float OhLTzvsPgtuSgEmyggHF43798000 = -271872195;    float OhLTzvsPgtuSgEmyggHF85163912 = -99065230;    float OhLTzvsPgtuSgEmyggHF1224235 = -203961631;    float OhLTzvsPgtuSgEmyggHF60788979 = -98012664;    float OhLTzvsPgtuSgEmyggHF67051321 = -885412575;    float OhLTzvsPgtuSgEmyggHF39953250 = -267361078;    float OhLTzvsPgtuSgEmyggHF4794219 = -984822641;    float OhLTzvsPgtuSgEmyggHF30807204 = -663752257;    float OhLTzvsPgtuSgEmyggHF67454614 = -682488519;    float OhLTzvsPgtuSgEmyggHF30066012 = -441800354;    float OhLTzvsPgtuSgEmyggHF93803692 = -200987062;    float OhLTzvsPgtuSgEmyggHF4854481 = -24298512;    float OhLTzvsPgtuSgEmyggHF84768827 = -214946784;    float OhLTzvsPgtuSgEmyggHF88037383 = -645092388;    float OhLTzvsPgtuSgEmyggHF41914241 = -290577006;    float OhLTzvsPgtuSgEmyggHF87898888 = -268252086;    float OhLTzvsPgtuSgEmyggHF76289692 = 5367180;    float OhLTzvsPgtuSgEmyggHF79356683 = 62582316;    float OhLTzvsPgtuSgEmyggHF89536370 = -733456044;    float OhLTzvsPgtuSgEmyggHF46422046 = -830857476;    float OhLTzvsPgtuSgEmyggHF71464136 = -86799449;    float OhLTzvsPgtuSgEmyggHF62868617 = 32507068;    float OhLTzvsPgtuSgEmyggHF34987548 = -631493741;    float OhLTzvsPgtuSgEmyggHF1249611 = -592502192;    float OhLTzvsPgtuSgEmyggHF91682711 = 22858622;    float OhLTzvsPgtuSgEmyggHF48963503 = -573180584;    float OhLTzvsPgtuSgEmyggHF83937232 = -927624221;    float OhLTzvsPgtuSgEmyggHF93879281 = -429287370;    float OhLTzvsPgtuSgEmyggHF3994433 = -576846121;    float OhLTzvsPgtuSgEmyggHF83354201 = -454271209;    float OhLTzvsPgtuSgEmyggHF61322547 = -598120562;    float OhLTzvsPgtuSgEmyggHF38673941 = -584029706;    float OhLTzvsPgtuSgEmyggHF16207145 = -376207862;    float OhLTzvsPgtuSgEmyggHF71303305 = -877983157;    float OhLTzvsPgtuSgEmyggHF65960755 = -222866472;    float OhLTzvsPgtuSgEmyggHF49551729 = -211391761;    float OhLTzvsPgtuSgEmyggHF24535874 = -811610092;    float OhLTzvsPgtuSgEmyggHF19408172 = -587290210;    float OhLTzvsPgtuSgEmyggHF72780768 = -782229258;    float OhLTzvsPgtuSgEmyggHF53541927 = 25921653;    float OhLTzvsPgtuSgEmyggHF53862637 = -428141777;    float OhLTzvsPgtuSgEmyggHF30871207 = -543021024;    float OhLTzvsPgtuSgEmyggHF54576774 = -274897861;    float OhLTzvsPgtuSgEmyggHF73511318 = -678585158;    float OhLTzvsPgtuSgEmyggHF72643461 = -863667842;    float OhLTzvsPgtuSgEmyggHF54840772 = -882572481;    float OhLTzvsPgtuSgEmyggHF95368683 = -414879470;    float OhLTzvsPgtuSgEmyggHF94443752 = -801586672;    float OhLTzvsPgtuSgEmyggHF74361360 = -7800553;    float OhLTzvsPgtuSgEmyggHF73780053 = -658962913;    float OhLTzvsPgtuSgEmyggHF23485059 = -432942477;    float OhLTzvsPgtuSgEmyggHF7955676 = -917939532;    float OhLTzvsPgtuSgEmyggHF66562420 = -491303979;    float OhLTzvsPgtuSgEmyggHF21424089 = -16696136;    float OhLTzvsPgtuSgEmyggHF91338351 = -620549751;    float OhLTzvsPgtuSgEmyggHF80929383 = -204379263;    float OhLTzvsPgtuSgEmyggHF50176365 = -467571489;    float OhLTzvsPgtuSgEmyggHF99974623 = -611459439;    float OhLTzvsPgtuSgEmyggHF69106268 = -20871286;    float OhLTzvsPgtuSgEmyggHF18087819 = -212231992;    float OhLTzvsPgtuSgEmyggHF56016017 = -339736857;    float OhLTzvsPgtuSgEmyggHF10914937 = -455535272;    float OhLTzvsPgtuSgEmyggHF26812772 = 13093864;    float OhLTzvsPgtuSgEmyggHF84100412 = -243821892;     OhLTzvsPgtuSgEmyggHF5078902 = OhLTzvsPgtuSgEmyggHF33142546;     OhLTzvsPgtuSgEmyggHF33142546 = OhLTzvsPgtuSgEmyggHF84900004;     OhLTzvsPgtuSgEmyggHF84900004 = OhLTzvsPgtuSgEmyggHF38564877;     OhLTzvsPgtuSgEmyggHF38564877 = OhLTzvsPgtuSgEmyggHF67696764;     OhLTzvsPgtuSgEmyggHF67696764 = OhLTzvsPgtuSgEmyggHF51377964;     OhLTzvsPgtuSgEmyggHF51377964 = OhLTzvsPgtuSgEmyggHF29101014;     OhLTzvsPgtuSgEmyggHF29101014 = OhLTzvsPgtuSgEmyggHF66418367;     OhLTzvsPgtuSgEmyggHF66418367 = OhLTzvsPgtuSgEmyggHF10154884;     OhLTzvsPgtuSgEmyggHF10154884 = OhLTzvsPgtuSgEmyggHF65673374;     OhLTzvsPgtuSgEmyggHF65673374 = OhLTzvsPgtuSgEmyggHF83128952;     OhLTzvsPgtuSgEmyggHF83128952 = OhLTzvsPgtuSgEmyggHF48634906;     OhLTzvsPgtuSgEmyggHF48634906 = OhLTzvsPgtuSgEmyggHF94772731;     OhLTzvsPgtuSgEmyggHF94772731 = OhLTzvsPgtuSgEmyggHF84053280;     OhLTzvsPgtuSgEmyggHF84053280 = OhLTzvsPgtuSgEmyggHF34105793;     OhLTzvsPgtuSgEmyggHF34105793 = OhLTzvsPgtuSgEmyggHF9758755;     OhLTzvsPgtuSgEmyggHF9758755 = OhLTzvsPgtuSgEmyggHF34715642;     OhLTzvsPgtuSgEmyggHF34715642 = OhLTzvsPgtuSgEmyggHF25760108;     OhLTzvsPgtuSgEmyggHF25760108 = OhLTzvsPgtuSgEmyggHF80197151;     OhLTzvsPgtuSgEmyggHF80197151 = OhLTzvsPgtuSgEmyggHF39832089;     OhLTzvsPgtuSgEmyggHF39832089 = OhLTzvsPgtuSgEmyggHF93495176;     OhLTzvsPgtuSgEmyggHF93495176 = OhLTzvsPgtuSgEmyggHF58656856;     OhLTzvsPgtuSgEmyggHF58656856 = OhLTzvsPgtuSgEmyggHF61678410;     OhLTzvsPgtuSgEmyggHF61678410 = OhLTzvsPgtuSgEmyggHF22031388;     OhLTzvsPgtuSgEmyggHF22031388 = OhLTzvsPgtuSgEmyggHF3577330;     OhLTzvsPgtuSgEmyggHF3577330 = OhLTzvsPgtuSgEmyggHF66447154;     OhLTzvsPgtuSgEmyggHF66447154 = OhLTzvsPgtuSgEmyggHF59695253;     OhLTzvsPgtuSgEmyggHF59695253 = OhLTzvsPgtuSgEmyggHF80137511;     OhLTzvsPgtuSgEmyggHF80137511 = OhLTzvsPgtuSgEmyggHF82481135;     OhLTzvsPgtuSgEmyggHF82481135 = OhLTzvsPgtuSgEmyggHF16275602;     OhLTzvsPgtuSgEmyggHF16275602 = OhLTzvsPgtuSgEmyggHF61678942;     OhLTzvsPgtuSgEmyggHF61678942 = OhLTzvsPgtuSgEmyggHF99774750;     OhLTzvsPgtuSgEmyggHF99774750 = OhLTzvsPgtuSgEmyggHF87312359;     OhLTzvsPgtuSgEmyggHF87312359 = OhLTzvsPgtuSgEmyggHF56098791;     OhLTzvsPgtuSgEmyggHF56098791 = OhLTzvsPgtuSgEmyggHF67846135;     OhLTzvsPgtuSgEmyggHF67846135 = OhLTzvsPgtuSgEmyggHF62802487;     OhLTzvsPgtuSgEmyggHF62802487 = OhLTzvsPgtuSgEmyggHF43798000;     OhLTzvsPgtuSgEmyggHF43798000 = OhLTzvsPgtuSgEmyggHF85163912;     OhLTzvsPgtuSgEmyggHF85163912 = OhLTzvsPgtuSgEmyggHF1224235;     OhLTzvsPgtuSgEmyggHF1224235 = OhLTzvsPgtuSgEmyggHF60788979;     OhLTzvsPgtuSgEmyggHF60788979 = OhLTzvsPgtuSgEmyggHF67051321;     OhLTzvsPgtuSgEmyggHF67051321 = OhLTzvsPgtuSgEmyggHF39953250;     OhLTzvsPgtuSgEmyggHF39953250 = OhLTzvsPgtuSgEmyggHF4794219;     OhLTzvsPgtuSgEmyggHF4794219 = OhLTzvsPgtuSgEmyggHF30807204;     OhLTzvsPgtuSgEmyggHF30807204 = OhLTzvsPgtuSgEmyggHF67454614;     OhLTzvsPgtuSgEmyggHF67454614 = OhLTzvsPgtuSgEmyggHF30066012;     OhLTzvsPgtuSgEmyggHF30066012 = OhLTzvsPgtuSgEmyggHF93803692;     OhLTzvsPgtuSgEmyggHF93803692 = OhLTzvsPgtuSgEmyggHF4854481;     OhLTzvsPgtuSgEmyggHF4854481 = OhLTzvsPgtuSgEmyggHF84768827;     OhLTzvsPgtuSgEmyggHF84768827 = OhLTzvsPgtuSgEmyggHF88037383;     OhLTzvsPgtuSgEmyggHF88037383 = OhLTzvsPgtuSgEmyggHF41914241;     OhLTzvsPgtuSgEmyggHF41914241 = OhLTzvsPgtuSgEmyggHF87898888;     OhLTzvsPgtuSgEmyggHF87898888 = OhLTzvsPgtuSgEmyggHF76289692;     OhLTzvsPgtuSgEmyggHF76289692 = OhLTzvsPgtuSgEmyggHF79356683;     OhLTzvsPgtuSgEmyggHF79356683 = OhLTzvsPgtuSgEmyggHF89536370;     OhLTzvsPgtuSgEmyggHF89536370 = OhLTzvsPgtuSgEmyggHF46422046;     OhLTzvsPgtuSgEmyggHF46422046 = OhLTzvsPgtuSgEmyggHF71464136;     OhLTzvsPgtuSgEmyggHF71464136 = OhLTzvsPgtuSgEmyggHF62868617;     OhLTzvsPgtuSgEmyggHF62868617 = OhLTzvsPgtuSgEmyggHF34987548;     OhLTzvsPgtuSgEmyggHF34987548 = OhLTzvsPgtuSgEmyggHF1249611;     OhLTzvsPgtuSgEmyggHF1249611 = OhLTzvsPgtuSgEmyggHF91682711;     OhLTzvsPgtuSgEmyggHF91682711 = OhLTzvsPgtuSgEmyggHF48963503;     OhLTzvsPgtuSgEmyggHF48963503 = OhLTzvsPgtuSgEmyggHF83937232;     OhLTzvsPgtuSgEmyggHF83937232 = OhLTzvsPgtuSgEmyggHF93879281;     OhLTzvsPgtuSgEmyggHF93879281 = OhLTzvsPgtuSgEmyggHF3994433;     OhLTzvsPgtuSgEmyggHF3994433 = OhLTzvsPgtuSgEmyggHF83354201;     OhLTzvsPgtuSgEmyggHF83354201 = OhLTzvsPgtuSgEmyggHF61322547;     OhLTzvsPgtuSgEmyggHF61322547 = OhLTzvsPgtuSgEmyggHF38673941;     OhLTzvsPgtuSgEmyggHF38673941 = OhLTzvsPgtuSgEmyggHF16207145;     OhLTzvsPgtuSgEmyggHF16207145 = OhLTzvsPgtuSgEmyggHF71303305;     OhLTzvsPgtuSgEmyggHF71303305 = OhLTzvsPgtuSgEmyggHF65960755;     OhLTzvsPgtuSgEmyggHF65960755 = OhLTzvsPgtuSgEmyggHF49551729;     OhLTzvsPgtuSgEmyggHF49551729 = OhLTzvsPgtuSgEmyggHF24535874;     OhLTzvsPgtuSgEmyggHF24535874 = OhLTzvsPgtuSgEmyggHF19408172;     OhLTzvsPgtuSgEmyggHF19408172 = OhLTzvsPgtuSgEmyggHF72780768;     OhLTzvsPgtuSgEmyggHF72780768 = OhLTzvsPgtuSgEmyggHF53541927;     OhLTzvsPgtuSgEmyggHF53541927 = OhLTzvsPgtuSgEmyggHF53862637;     OhLTzvsPgtuSgEmyggHF53862637 = OhLTzvsPgtuSgEmyggHF30871207;     OhLTzvsPgtuSgEmyggHF30871207 = OhLTzvsPgtuSgEmyggHF54576774;     OhLTzvsPgtuSgEmyggHF54576774 = OhLTzvsPgtuSgEmyggHF73511318;     OhLTzvsPgtuSgEmyggHF73511318 = OhLTzvsPgtuSgEmyggHF72643461;     OhLTzvsPgtuSgEmyggHF72643461 = OhLTzvsPgtuSgEmyggHF54840772;     OhLTzvsPgtuSgEmyggHF54840772 = OhLTzvsPgtuSgEmyggHF95368683;     OhLTzvsPgtuSgEmyggHF95368683 = OhLTzvsPgtuSgEmyggHF94443752;     OhLTzvsPgtuSgEmyggHF94443752 = OhLTzvsPgtuSgEmyggHF74361360;     OhLTzvsPgtuSgEmyggHF74361360 = OhLTzvsPgtuSgEmyggHF73780053;     OhLTzvsPgtuSgEmyggHF73780053 = OhLTzvsPgtuSgEmyggHF23485059;     OhLTzvsPgtuSgEmyggHF23485059 = OhLTzvsPgtuSgEmyggHF7955676;     OhLTzvsPgtuSgEmyggHF7955676 = OhLTzvsPgtuSgEmyggHF66562420;     OhLTzvsPgtuSgEmyggHF66562420 = OhLTzvsPgtuSgEmyggHF21424089;     OhLTzvsPgtuSgEmyggHF21424089 = OhLTzvsPgtuSgEmyggHF91338351;     OhLTzvsPgtuSgEmyggHF91338351 = OhLTzvsPgtuSgEmyggHF80929383;     OhLTzvsPgtuSgEmyggHF80929383 = OhLTzvsPgtuSgEmyggHF50176365;     OhLTzvsPgtuSgEmyggHF50176365 = OhLTzvsPgtuSgEmyggHF99974623;     OhLTzvsPgtuSgEmyggHF99974623 = OhLTzvsPgtuSgEmyggHF69106268;     OhLTzvsPgtuSgEmyggHF69106268 = OhLTzvsPgtuSgEmyggHF18087819;     OhLTzvsPgtuSgEmyggHF18087819 = OhLTzvsPgtuSgEmyggHF56016017;     OhLTzvsPgtuSgEmyggHF56016017 = OhLTzvsPgtuSgEmyggHF10914937;     OhLTzvsPgtuSgEmyggHF10914937 = OhLTzvsPgtuSgEmyggHF26812772;     OhLTzvsPgtuSgEmyggHF26812772 = OhLTzvsPgtuSgEmyggHF84100412;     OhLTzvsPgtuSgEmyggHF84100412 = OhLTzvsPgtuSgEmyggHF5078902;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void jEpVLTzMjnVBstRzljFB89690291() {     float OHoYjWjoLZwqwOsehXxx5072431 = -116346034;    float OHoYjWjoLZwqwOsehXxx71714862 = -436167854;    float OHoYjWjoLZwqwOsehXxx86213216 = 17992074;    float OHoYjWjoLZwqwOsehXxx23644738 = -672420084;    float OHoYjWjoLZwqwOsehXxx76536866 = -125512417;    float OHoYjWjoLZwqwOsehXxx86183396 = -783445182;    float OHoYjWjoLZwqwOsehXxx97648349 = -7543303;    float OHoYjWjoLZwqwOsehXxx35632418 = -78480597;    float OHoYjWjoLZwqwOsehXxx92895047 = -641310998;    float OHoYjWjoLZwqwOsehXxx62312888 = -240867989;    float OHoYjWjoLZwqwOsehXxx3449558 = 58681423;    float OHoYjWjoLZwqwOsehXxx62926583 = -288956140;    float OHoYjWjoLZwqwOsehXxx26625646 = -930448202;    float OHoYjWjoLZwqwOsehXxx37897500 = -668914525;    float OHoYjWjoLZwqwOsehXxx79277902 = -648045208;    float OHoYjWjoLZwqwOsehXxx84323302 = -927534317;    float OHoYjWjoLZwqwOsehXxx90568401 = -230071560;    float OHoYjWjoLZwqwOsehXxx48760641 = -412643344;    float OHoYjWjoLZwqwOsehXxx625422 = -303285140;    float OHoYjWjoLZwqwOsehXxx11905852 = -179666284;    float OHoYjWjoLZwqwOsehXxx73164600 = -51905798;    float OHoYjWjoLZwqwOsehXxx36953122 = -922956300;    float OHoYjWjoLZwqwOsehXxx98556806 = -665731122;    float OHoYjWjoLZwqwOsehXxx65681496 = -938306903;    float OHoYjWjoLZwqwOsehXxx32020019 = -547079366;    float OHoYjWjoLZwqwOsehXxx83021230 = 27139169;    float OHoYjWjoLZwqwOsehXxx29916834 = -207670705;    float OHoYjWjoLZwqwOsehXxx15541036 = -238793242;    float OHoYjWjoLZwqwOsehXxx95159582 = 7250626;    float OHoYjWjoLZwqwOsehXxx92365471 = -73808933;    float OHoYjWjoLZwqwOsehXxx95091620 = -314308233;    float OHoYjWjoLZwqwOsehXxx84498607 = -919397058;    float OHoYjWjoLZwqwOsehXxx72764782 = -57301431;    float OHoYjWjoLZwqwOsehXxx56381628 = -101441755;    float OHoYjWjoLZwqwOsehXxx50204173 = -106867536;    float OHoYjWjoLZwqwOsehXxx5630678 = -887114946;    float OHoYjWjoLZwqwOsehXxx55739367 = -371525573;    float OHoYjWjoLZwqwOsehXxx55081592 = -94327909;    float OHoYjWjoLZwqwOsehXxx36537831 = -916643047;    float OHoYjWjoLZwqwOsehXxx32661872 = -807444350;    float OHoYjWjoLZwqwOsehXxx54906989 = -973356473;    float OHoYjWjoLZwqwOsehXxx25650911 = -461153160;    float OHoYjWjoLZwqwOsehXxx65077578 = -443952895;    float OHoYjWjoLZwqwOsehXxx90039102 = -220716700;    float OHoYjWjoLZwqwOsehXxx51472666 = -55747921;    float OHoYjWjoLZwqwOsehXxx70493979 = -253845025;    float OHoYjWjoLZwqwOsehXxx21501485 = -682267684;    float OHoYjWjoLZwqwOsehXxx80238061 = -633059868;    float OHoYjWjoLZwqwOsehXxx3427218 = -878710645;    float OHoYjWjoLZwqwOsehXxx8082807 = -232117274;    float OHoYjWjoLZwqwOsehXxx55173611 = -969132014;    float OHoYjWjoLZwqwOsehXxx2446146 = -650397834;    float OHoYjWjoLZwqwOsehXxx51738040 = -986686828;    float OHoYjWjoLZwqwOsehXxx89606954 = -469337535;    float OHoYjWjoLZwqwOsehXxx16525691 = -490355321;    float OHoYjWjoLZwqwOsehXxx68119309 = -193389734;    float OHoYjWjoLZwqwOsehXxx73158055 = -770436732;    float OHoYjWjoLZwqwOsehXxx20531721 = -43701024;    float OHoYjWjoLZwqwOsehXxx91624718 = -25340718;    float OHoYjWjoLZwqwOsehXxx93515635 = -52651586;    float OHoYjWjoLZwqwOsehXxx56266562 = -475774477;    float OHoYjWjoLZwqwOsehXxx82107313 = -768750062;    float OHoYjWjoLZwqwOsehXxx40472836 = 14268777;    float OHoYjWjoLZwqwOsehXxx529576 = -467502066;    float OHoYjWjoLZwqwOsehXxx67221267 = -926559757;    float OHoYjWjoLZwqwOsehXxx18950950 = -21921520;    float OHoYjWjoLZwqwOsehXxx90161800 = -131654709;    float OHoYjWjoLZwqwOsehXxx70244017 = -729006448;    float OHoYjWjoLZwqwOsehXxx87693327 = -462046989;    float OHoYjWjoLZwqwOsehXxx73647224 = -760930263;    float OHoYjWjoLZwqwOsehXxx28583935 = -456008744;    float OHoYjWjoLZwqwOsehXxx35486810 = -35743652;    float OHoYjWjoLZwqwOsehXxx12222810 = -496000298;    float OHoYjWjoLZwqwOsehXxx67963549 = -495840790;    float OHoYjWjoLZwqwOsehXxx56998863 = -206309811;    float OHoYjWjoLZwqwOsehXxx47513690 = -590752639;    float OHoYjWjoLZwqwOsehXxx71875543 = -379003406;    float OHoYjWjoLZwqwOsehXxx8517705 = -345014422;    float OHoYjWjoLZwqwOsehXxx14208830 = -782558982;    float OHoYjWjoLZwqwOsehXxx61526039 = -193234341;    float OHoYjWjoLZwqwOsehXxx61519746 = -290593148;    float OHoYjWjoLZwqwOsehXxx49678773 = -574610838;    float OHoYjWjoLZwqwOsehXxx12113819 = -360082597;    float OHoYjWjoLZwqwOsehXxx87076776 = -760632100;    float OHoYjWjoLZwqwOsehXxx37191861 = -104676919;    float OHoYjWjoLZwqwOsehXxx92645475 = -663910399;    float OHoYjWjoLZwqwOsehXxx32760568 = -932710230;    float OHoYjWjoLZwqwOsehXxx83157828 = -587963897;    float OHoYjWjoLZwqwOsehXxx39855938 = -611086434;    float OHoYjWjoLZwqwOsehXxx82084864 = -913477803;    float OHoYjWjoLZwqwOsehXxx32472622 = -16678214;    float OHoYjWjoLZwqwOsehXxx35207647 = -227824550;    float OHoYjWjoLZwqwOsehXxx63456873 = 31012809;    float OHoYjWjoLZwqwOsehXxx43022195 = -763991462;    float OHoYjWjoLZwqwOsehXxx76395310 = -231669874;    float OHoYjWjoLZwqwOsehXxx72799675 = -104606411;    float OHoYjWjoLZwqwOsehXxx85178074 = -375421937;    float OHoYjWjoLZwqwOsehXxx64548003 = -976450830;    float OHoYjWjoLZwqwOsehXxx22817835 = -294156943;    float OHoYjWjoLZwqwOsehXxx32521717 = -116346034;     OHoYjWjoLZwqwOsehXxx5072431 = OHoYjWjoLZwqwOsehXxx71714862;     OHoYjWjoLZwqwOsehXxx71714862 = OHoYjWjoLZwqwOsehXxx86213216;     OHoYjWjoLZwqwOsehXxx86213216 = OHoYjWjoLZwqwOsehXxx23644738;     OHoYjWjoLZwqwOsehXxx23644738 = OHoYjWjoLZwqwOsehXxx76536866;     OHoYjWjoLZwqwOsehXxx76536866 = OHoYjWjoLZwqwOsehXxx86183396;     OHoYjWjoLZwqwOsehXxx86183396 = OHoYjWjoLZwqwOsehXxx97648349;     OHoYjWjoLZwqwOsehXxx97648349 = OHoYjWjoLZwqwOsehXxx35632418;     OHoYjWjoLZwqwOsehXxx35632418 = OHoYjWjoLZwqwOsehXxx92895047;     OHoYjWjoLZwqwOsehXxx92895047 = OHoYjWjoLZwqwOsehXxx62312888;     OHoYjWjoLZwqwOsehXxx62312888 = OHoYjWjoLZwqwOsehXxx3449558;     OHoYjWjoLZwqwOsehXxx3449558 = OHoYjWjoLZwqwOsehXxx62926583;     OHoYjWjoLZwqwOsehXxx62926583 = OHoYjWjoLZwqwOsehXxx26625646;     OHoYjWjoLZwqwOsehXxx26625646 = OHoYjWjoLZwqwOsehXxx37897500;     OHoYjWjoLZwqwOsehXxx37897500 = OHoYjWjoLZwqwOsehXxx79277902;     OHoYjWjoLZwqwOsehXxx79277902 = OHoYjWjoLZwqwOsehXxx84323302;     OHoYjWjoLZwqwOsehXxx84323302 = OHoYjWjoLZwqwOsehXxx90568401;     OHoYjWjoLZwqwOsehXxx90568401 = OHoYjWjoLZwqwOsehXxx48760641;     OHoYjWjoLZwqwOsehXxx48760641 = OHoYjWjoLZwqwOsehXxx625422;     OHoYjWjoLZwqwOsehXxx625422 = OHoYjWjoLZwqwOsehXxx11905852;     OHoYjWjoLZwqwOsehXxx11905852 = OHoYjWjoLZwqwOsehXxx73164600;     OHoYjWjoLZwqwOsehXxx73164600 = OHoYjWjoLZwqwOsehXxx36953122;     OHoYjWjoLZwqwOsehXxx36953122 = OHoYjWjoLZwqwOsehXxx98556806;     OHoYjWjoLZwqwOsehXxx98556806 = OHoYjWjoLZwqwOsehXxx65681496;     OHoYjWjoLZwqwOsehXxx65681496 = OHoYjWjoLZwqwOsehXxx32020019;     OHoYjWjoLZwqwOsehXxx32020019 = OHoYjWjoLZwqwOsehXxx83021230;     OHoYjWjoLZwqwOsehXxx83021230 = OHoYjWjoLZwqwOsehXxx29916834;     OHoYjWjoLZwqwOsehXxx29916834 = OHoYjWjoLZwqwOsehXxx15541036;     OHoYjWjoLZwqwOsehXxx15541036 = OHoYjWjoLZwqwOsehXxx95159582;     OHoYjWjoLZwqwOsehXxx95159582 = OHoYjWjoLZwqwOsehXxx92365471;     OHoYjWjoLZwqwOsehXxx92365471 = OHoYjWjoLZwqwOsehXxx95091620;     OHoYjWjoLZwqwOsehXxx95091620 = OHoYjWjoLZwqwOsehXxx84498607;     OHoYjWjoLZwqwOsehXxx84498607 = OHoYjWjoLZwqwOsehXxx72764782;     OHoYjWjoLZwqwOsehXxx72764782 = OHoYjWjoLZwqwOsehXxx56381628;     OHoYjWjoLZwqwOsehXxx56381628 = OHoYjWjoLZwqwOsehXxx50204173;     OHoYjWjoLZwqwOsehXxx50204173 = OHoYjWjoLZwqwOsehXxx5630678;     OHoYjWjoLZwqwOsehXxx5630678 = OHoYjWjoLZwqwOsehXxx55739367;     OHoYjWjoLZwqwOsehXxx55739367 = OHoYjWjoLZwqwOsehXxx55081592;     OHoYjWjoLZwqwOsehXxx55081592 = OHoYjWjoLZwqwOsehXxx36537831;     OHoYjWjoLZwqwOsehXxx36537831 = OHoYjWjoLZwqwOsehXxx32661872;     OHoYjWjoLZwqwOsehXxx32661872 = OHoYjWjoLZwqwOsehXxx54906989;     OHoYjWjoLZwqwOsehXxx54906989 = OHoYjWjoLZwqwOsehXxx25650911;     OHoYjWjoLZwqwOsehXxx25650911 = OHoYjWjoLZwqwOsehXxx65077578;     OHoYjWjoLZwqwOsehXxx65077578 = OHoYjWjoLZwqwOsehXxx90039102;     OHoYjWjoLZwqwOsehXxx90039102 = OHoYjWjoLZwqwOsehXxx51472666;     OHoYjWjoLZwqwOsehXxx51472666 = OHoYjWjoLZwqwOsehXxx70493979;     OHoYjWjoLZwqwOsehXxx70493979 = OHoYjWjoLZwqwOsehXxx21501485;     OHoYjWjoLZwqwOsehXxx21501485 = OHoYjWjoLZwqwOsehXxx80238061;     OHoYjWjoLZwqwOsehXxx80238061 = OHoYjWjoLZwqwOsehXxx3427218;     OHoYjWjoLZwqwOsehXxx3427218 = OHoYjWjoLZwqwOsehXxx8082807;     OHoYjWjoLZwqwOsehXxx8082807 = OHoYjWjoLZwqwOsehXxx55173611;     OHoYjWjoLZwqwOsehXxx55173611 = OHoYjWjoLZwqwOsehXxx2446146;     OHoYjWjoLZwqwOsehXxx2446146 = OHoYjWjoLZwqwOsehXxx51738040;     OHoYjWjoLZwqwOsehXxx51738040 = OHoYjWjoLZwqwOsehXxx89606954;     OHoYjWjoLZwqwOsehXxx89606954 = OHoYjWjoLZwqwOsehXxx16525691;     OHoYjWjoLZwqwOsehXxx16525691 = OHoYjWjoLZwqwOsehXxx68119309;     OHoYjWjoLZwqwOsehXxx68119309 = OHoYjWjoLZwqwOsehXxx73158055;     OHoYjWjoLZwqwOsehXxx73158055 = OHoYjWjoLZwqwOsehXxx20531721;     OHoYjWjoLZwqwOsehXxx20531721 = OHoYjWjoLZwqwOsehXxx91624718;     OHoYjWjoLZwqwOsehXxx91624718 = OHoYjWjoLZwqwOsehXxx93515635;     OHoYjWjoLZwqwOsehXxx93515635 = OHoYjWjoLZwqwOsehXxx56266562;     OHoYjWjoLZwqwOsehXxx56266562 = OHoYjWjoLZwqwOsehXxx82107313;     OHoYjWjoLZwqwOsehXxx82107313 = OHoYjWjoLZwqwOsehXxx40472836;     OHoYjWjoLZwqwOsehXxx40472836 = OHoYjWjoLZwqwOsehXxx529576;     OHoYjWjoLZwqwOsehXxx529576 = OHoYjWjoLZwqwOsehXxx67221267;     OHoYjWjoLZwqwOsehXxx67221267 = OHoYjWjoLZwqwOsehXxx18950950;     OHoYjWjoLZwqwOsehXxx18950950 = OHoYjWjoLZwqwOsehXxx90161800;     OHoYjWjoLZwqwOsehXxx90161800 = OHoYjWjoLZwqwOsehXxx70244017;     OHoYjWjoLZwqwOsehXxx70244017 = OHoYjWjoLZwqwOsehXxx87693327;     OHoYjWjoLZwqwOsehXxx87693327 = OHoYjWjoLZwqwOsehXxx73647224;     OHoYjWjoLZwqwOsehXxx73647224 = OHoYjWjoLZwqwOsehXxx28583935;     OHoYjWjoLZwqwOsehXxx28583935 = OHoYjWjoLZwqwOsehXxx35486810;     OHoYjWjoLZwqwOsehXxx35486810 = OHoYjWjoLZwqwOsehXxx12222810;     OHoYjWjoLZwqwOsehXxx12222810 = OHoYjWjoLZwqwOsehXxx67963549;     OHoYjWjoLZwqwOsehXxx67963549 = OHoYjWjoLZwqwOsehXxx56998863;     OHoYjWjoLZwqwOsehXxx56998863 = OHoYjWjoLZwqwOsehXxx47513690;     OHoYjWjoLZwqwOsehXxx47513690 = OHoYjWjoLZwqwOsehXxx71875543;     OHoYjWjoLZwqwOsehXxx71875543 = OHoYjWjoLZwqwOsehXxx8517705;     OHoYjWjoLZwqwOsehXxx8517705 = OHoYjWjoLZwqwOsehXxx14208830;     OHoYjWjoLZwqwOsehXxx14208830 = OHoYjWjoLZwqwOsehXxx61526039;     OHoYjWjoLZwqwOsehXxx61526039 = OHoYjWjoLZwqwOsehXxx61519746;     OHoYjWjoLZwqwOsehXxx61519746 = OHoYjWjoLZwqwOsehXxx49678773;     OHoYjWjoLZwqwOsehXxx49678773 = OHoYjWjoLZwqwOsehXxx12113819;     OHoYjWjoLZwqwOsehXxx12113819 = OHoYjWjoLZwqwOsehXxx87076776;     OHoYjWjoLZwqwOsehXxx87076776 = OHoYjWjoLZwqwOsehXxx37191861;     OHoYjWjoLZwqwOsehXxx37191861 = OHoYjWjoLZwqwOsehXxx92645475;     OHoYjWjoLZwqwOsehXxx92645475 = OHoYjWjoLZwqwOsehXxx32760568;     OHoYjWjoLZwqwOsehXxx32760568 = OHoYjWjoLZwqwOsehXxx83157828;     OHoYjWjoLZwqwOsehXxx83157828 = OHoYjWjoLZwqwOsehXxx39855938;     OHoYjWjoLZwqwOsehXxx39855938 = OHoYjWjoLZwqwOsehXxx82084864;     OHoYjWjoLZwqwOsehXxx82084864 = OHoYjWjoLZwqwOsehXxx32472622;     OHoYjWjoLZwqwOsehXxx32472622 = OHoYjWjoLZwqwOsehXxx35207647;     OHoYjWjoLZwqwOsehXxx35207647 = OHoYjWjoLZwqwOsehXxx63456873;     OHoYjWjoLZwqwOsehXxx63456873 = OHoYjWjoLZwqwOsehXxx43022195;     OHoYjWjoLZwqwOsehXxx43022195 = OHoYjWjoLZwqwOsehXxx76395310;     OHoYjWjoLZwqwOsehXxx76395310 = OHoYjWjoLZwqwOsehXxx72799675;     OHoYjWjoLZwqwOsehXxx72799675 = OHoYjWjoLZwqwOsehXxx85178074;     OHoYjWjoLZwqwOsehXxx85178074 = OHoYjWjoLZwqwOsehXxx64548003;     OHoYjWjoLZwqwOsehXxx64548003 = OHoYjWjoLZwqwOsehXxx22817835;     OHoYjWjoLZwqwOsehXxx22817835 = OHoYjWjoLZwqwOsehXxx32521717;     OHoYjWjoLZwqwOsehXxx32521717 = OHoYjWjoLZwqwOsehXxx5072431;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void oaFuqSamvuylBuRCmhjK65702411() {     float ajamnfdfMcSuXsZgURHn91994108 = -256997238;    float ajamnfdfMcSuXsZgURHn81362601 = -546754427;    float ajamnfdfMcSuXsZgURHn78909230 = -513123309;    float ajamnfdfMcSuXsZgURHn51507932 = -678217374;    float ajamnfdfMcSuXsZgURHn27265954 = 20674106;    float ajamnfdfMcSuXsZgURHn99649073 = -81138476;    float ajamnfdfMcSuXsZgURHn47694018 = -789341342;    float ajamnfdfMcSuXsZgURHn18348767 = -609020418;    float ajamnfdfMcSuXsZgURHn71280737 = -850821700;    float ajamnfdfMcSuXsZgURHn35813274 = 44270805;    float ajamnfdfMcSuXsZgURHn23130288 = -95407321;    float ajamnfdfMcSuXsZgURHn32553061 = -411059024;    float ajamnfdfMcSuXsZgURHn87252094 = 81188509;    float ajamnfdfMcSuXsZgURHn81215609 = -465412924;    float ajamnfdfMcSuXsZgURHn2990772 = -834810758;    float ajamnfdfMcSuXsZgURHn19602461 = -906185903;    float ajamnfdfMcSuXsZgURHn40408577 = -899991397;    float ajamnfdfMcSuXsZgURHn77977398 = -88029588;    float ajamnfdfMcSuXsZgURHn61784992 = -739806111;    float ajamnfdfMcSuXsZgURHn68658679 = -617764123;    float ajamnfdfMcSuXsZgURHn40630269 = -96584792;    float ajamnfdfMcSuXsZgURHn75643081 = -751078202;    float ajamnfdfMcSuXsZgURHn68203147 = -563137372;    float ajamnfdfMcSuXsZgURHn42038446 = -872395678;    float ajamnfdfMcSuXsZgURHn87985004 = -571347145;    float ajamnfdfMcSuXsZgURHn77200130 = -636384177;    float ajamnfdfMcSuXsZgURHn86807262 = -459772770;    float ajamnfdfMcSuXsZgURHn16127587 = -466442509;    float ajamnfdfMcSuXsZgURHn68062375 = -408982005;    float ajamnfdfMcSuXsZgURHn72079110 = -735029002;    float ajamnfdfMcSuXsZgURHn54211873 = -230737132;    float ajamnfdfMcSuXsZgURHn88940637 = 9681240;    float ajamnfdfMcSuXsZgURHn27037865 = -640993165;    float ajamnfdfMcSuXsZgURHn86304726 = -237556524;    float ajamnfdfMcSuXsZgURHn42983122 = -478837633;    float ajamnfdfMcSuXsZgURHn38590892 = 32289238;    float ajamnfdfMcSuXsZgURHn11708631 = -339430863;    float ajamnfdfMcSuXsZgURHn27409969 = 25439521;    float ajamnfdfMcSuXsZgURHn89768997 = -179494218;    float ajamnfdfMcSuXsZgURHn60448947 = 12162480;    float ajamnfdfMcSuXsZgURHn38068420 = -695279123;    float ajamnfdfMcSuXsZgURHn10491836 = -818308642;    float ajamnfdfMcSuXsZgURHn41527752 = -237432947;    float ajamnfdfMcSuXsZgURHn8583782 = -270712383;    float ajamnfdfMcSuXsZgURHn3457527 = -511639639;    float ajamnfdfMcSuXsZgURHn73060901 = -209195159;    float ajamnfdfMcSuXsZgURHn42151511 = -157972198;    float ajamnfdfMcSuXsZgURHn82494005 = -831829622;    float ajamnfdfMcSuXsZgURHn93652845 = -728454325;    float ajamnfdfMcSuXsZgURHn682386 = -570135604;    float ajamnfdfMcSuXsZgURHn96531473 = 71513260;    float ajamnfdfMcSuXsZgURHn7359188 = -149710518;    float ajamnfdfMcSuXsZgURHn36879643 = -794606513;    float ajamnfdfMcSuXsZgURHn44151924 = 14570305;    float ajamnfdfMcSuXsZgURHn25072143 = -41468569;    float ajamnfdfMcSuXsZgURHn16351027 = -505919036;    float ajamnfdfMcSuXsZgURHn13159454 = -983617056;    float ajamnfdfMcSuXsZgURHn36870784 = -640727631;    float ajamnfdfMcSuXsZgURHn63522928 = -6870230;    float ajamnfdfMcSuXsZgURHn50065823 = -342941718;    float ajamnfdfMcSuXsZgURHn12841812 = -621365706;    float ajamnfdfMcSuXsZgURHn31566432 = -222898833;    float ajamnfdfMcSuXsZgURHn50286391 = -100038413;    float ajamnfdfMcSuXsZgURHn99201626 = -15792698;    float ajamnfdfMcSuXsZgURHn81601401 = -724992064;    float ajamnfdfMcSuXsZgURHn34189650 = -5088561;    float ajamnfdfMcSuXsZgURHn5515196 = -770065860;    float ajamnfdfMcSuXsZgURHn947369 = -681254968;    float ajamnfdfMcSuXsZgURHn38232488 = -986575292;    float ajamnfdfMcSuXsZgURHn64399879 = -767099996;    float ajamnfdfMcSuXsZgURHn7893831 = -466755041;    float ajamnfdfMcSuXsZgURHn12998608 = -825430919;    float ajamnfdfMcSuXsZgURHn88208400 = -908535370;    float ajamnfdfMcSuXsZgURHn1336045 = -651968592;    float ajamnfdfMcSuXsZgURHn30590260 = -922485000;    float ajamnfdfMcSuXsZgURHn30138433 = -278276150;    float ajamnfdfMcSuXsZgURHn34115329 = -413645256;    float ajamnfdfMcSuXsZgURHn59619365 = -192424990;    float ajamnfdfMcSuXsZgURHn38580920 = -260756040;    float ajamnfdfMcSuXsZgURHn14924103 = -262151986;    float ajamnfdfMcSuXsZgURHn35048619 = -378411979;    float ajamnfdfMcSuXsZgURHn4313258 = -627943149;    float ajamnfdfMcSuXsZgURHn22474741 = -737988185;    float ajamnfdfMcSuXsZgURHn67379990 = -838846402;    float ajamnfdfMcSuXsZgURHn75547637 = -706542262;    float ajamnfdfMcSuXsZgURHn46852685 = 18973385;    float ajamnfdfMcSuXsZgURHn52060995 = -195712248;    float ajamnfdfMcSuXsZgURHn82885941 = -555563470;    float ajamnfdfMcSuXsZgURHn61232584 = -96087956;    float ajamnfdfMcSuXsZgURHn26632095 = -972918597;    float ajamnfdfMcSuXsZgURHn25431438 = 15906293;    float ajamnfdfMcSuXsZgURHn74837846 = -698703232;    float ajamnfdfMcSuXsZgURHn63887041 = -967690250;    float ajamnfdfMcSuXsZgURHn39703174 = -836552501;    float ajamnfdfMcSuXsZgURHn47607136 = -366471814;    float ajamnfdfMcSuXsZgURHn6501988 = -372380290;    float ajamnfdfMcSuXsZgURHn60205445 = -618270230;    float ajamnfdfMcSuXsZgURHn42326125 = -121640249;    float ajamnfdfMcSuXsZgURHn26982381 = -545720319;    float ajamnfdfMcSuXsZgURHn69267876 = -256997238;     ajamnfdfMcSuXsZgURHn91994108 = ajamnfdfMcSuXsZgURHn81362601;     ajamnfdfMcSuXsZgURHn81362601 = ajamnfdfMcSuXsZgURHn78909230;     ajamnfdfMcSuXsZgURHn78909230 = ajamnfdfMcSuXsZgURHn51507932;     ajamnfdfMcSuXsZgURHn51507932 = ajamnfdfMcSuXsZgURHn27265954;     ajamnfdfMcSuXsZgURHn27265954 = ajamnfdfMcSuXsZgURHn99649073;     ajamnfdfMcSuXsZgURHn99649073 = ajamnfdfMcSuXsZgURHn47694018;     ajamnfdfMcSuXsZgURHn47694018 = ajamnfdfMcSuXsZgURHn18348767;     ajamnfdfMcSuXsZgURHn18348767 = ajamnfdfMcSuXsZgURHn71280737;     ajamnfdfMcSuXsZgURHn71280737 = ajamnfdfMcSuXsZgURHn35813274;     ajamnfdfMcSuXsZgURHn35813274 = ajamnfdfMcSuXsZgURHn23130288;     ajamnfdfMcSuXsZgURHn23130288 = ajamnfdfMcSuXsZgURHn32553061;     ajamnfdfMcSuXsZgURHn32553061 = ajamnfdfMcSuXsZgURHn87252094;     ajamnfdfMcSuXsZgURHn87252094 = ajamnfdfMcSuXsZgURHn81215609;     ajamnfdfMcSuXsZgURHn81215609 = ajamnfdfMcSuXsZgURHn2990772;     ajamnfdfMcSuXsZgURHn2990772 = ajamnfdfMcSuXsZgURHn19602461;     ajamnfdfMcSuXsZgURHn19602461 = ajamnfdfMcSuXsZgURHn40408577;     ajamnfdfMcSuXsZgURHn40408577 = ajamnfdfMcSuXsZgURHn77977398;     ajamnfdfMcSuXsZgURHn77977398 = ajamnfdfMcSuXsZgURHn61784992;     ajamnfdfMcSuXsZgURHn61784992 = ajamnfdfMcSuXsZgURHn68658679;     ajamnfdfMcSuXsZgURHn68658679 = ajamnfdfMcSuXsZgURHn40630269;     ajamnfdfMcSuXsZgURHn40630269 = ajamnfdfMcSuXsZgURHn75643081;     ajamnfdfMcSuXsZgURHn75643081 = ajamnfdfMcSuXsZgURHn68203147;     ajamnfdfMcSuXsZgURHn68203147 = ajamnfdfMcSuXsZgURHn42038446;     ajamnfdfMcSuXsZgURHn42038446 = ajamnfdfMcSuXsZgURHn87985004;     ajamnfdfMcSuXsZgURHn87985004 = ajamnfdfMcSuXsZgURHn77200130;     ajamnfdfMcSuXsZgURHn77200130 = ajamnfdfMcSuXsZgURHn86807262;     ajamnfdfMcSuXsZgURHn86807262 = ajamnfdfMcSuXsZgURHn16127587;     ajamnfdfMcSuXsZgURHn16127587 = ajamnfdfMcSuXsZgURHn68062375;     ajamnfdfMcSuXsZgURHn68062375 = ajamnfdfMcSuXsZgURHn72079110;     ajamnfdfMcSuXsZgURHn72079110 = ajamnfdfMcSuXsZgURHn54211873;     ajamnfdfMcSuXsZgURHn54211873 = ajamnfdfMcSuXsZgURHn88940637;     ajamnfdfMcSuXsZgURHn88940637 = ajamnfdfMcSuXsZgURHn27037865;     ajamnfdfMcSuXsZgURHn27037865 = ajamnfdfMcSuXsZgURHn86304726;     ajamnfdfMcSuXsZgURHn86304726 = ajamnfdfMcSuXsZgURHn42983122;     ajamnfdfMcSuXsZgURHn42983122 = ajamnfdfMcSuXsZgURHn38590892;     ajamnfdfMcSuXsZgURHn38590892 = ajamnfdfMcSuXsZgURHn11708631;     ajamnfdfMcSuXsZgURHn11708631 = ajamnfdfMcSuXsZgURHn27409969;     ajamnfdfMcSuXsZgURHn27409969 = ajamnfdfMcSuXsZgURHn89768997;     ajamnfdfMcSuXsZgURHn89768997 = ajamnfdfMcSuXsZgURHn60448947;     ajamnfdfMcSuXsZgURHn60448947 = ajamnfdfMcSuXsZgURHn38068420;     ajamnfdfMcSuXsZgURHn38068420 = ajamnfdfMcSuXsZgURHn10491836;     ajamnfdfMcSuXsZgURHn10491836 = ajamnfdfMcSuXsZgURHn41527752;     ajamnfdfMcSuXsZgURHn41527752 = ajamnfdfMcSuXsZgURHn8583782;     ajamnfdfMcSuXsZgURHn8583782 = ajamnfdfMcSuXsZgURHn3457527;     ajamnfdfMcSuXsZgURHn3457527 = ajamnfdfMcSuXsZgURHn73060901;     ajamnfdfMcSuXsZgURHn73060901 = ajamnfdfMcSuXsZgURHn42151511;     ajamnfdfMcSuXsZgURHn42151511 = ajamnfdfMcSuXsZgURHn82494005;     ajamnfdfMcSuXsZgURHn82494005 = ajamnfdfMcSuXsZgURHn93652845;     ajamnfdfMcSuXsZgURHn93652845 = ajamnfdfMcSuXsZgURHn682386;     ajamnfdfMcSuXsZgURHn682386 = ajamnfdfMcSuXsZgURHn96531473;     ajamnfdfMcSuXsZgURHn96531473 = ajamnfdfMcSuXsZgURHn7359188;     ajamnfdfMcSuXsZgURHn7359188 = ajamnfdfMcSuXsZgURHn36879643;     ajamnfdfMcSuXsZgURHn36879643 = ajamnfdfMcSuXsZgURHn44151924;     ajamnfdfMcSuXsZgURHn44151924 = ajamnfdfMcSuXsZgURHn25072143;     ajamnfdfMcSuXsZgURHn25072143 = ajamnfdfMcSuXsZgURHn16351027;     ajamnfdfMcSuXsZgURHn16351027 = ajamnfdfMcSuXsZgURHn13159454;     ajamnfdfMcSuXsZgURHn13159454 = ajamnfdfMcSuXsZgURHn36870784;     ajamnfdfMcSuXsZgURHn36870784 = ajamnfdfMcSuXsZgURHn63522928;     ajamnfdfMcSuXsZgURHn63522928 = ajamnfdfMcSuXsZgURHn50065823;     ajamnfdfMcSuXsZgURHn50065823 = ajamnfdfMcSuXsZgURHn12841812;     ajamnfdfMcSuXsZgURHn12841812 = ajamnfdfMcSuXsZgURHn31566432;     ajamnfdfMcSuXsZgURHn31566432 = ajamnfdfMcSuXsZgURHn50286391;     ajamnfdfMcSuXsZgURHn50286391 = ajamnfdfMcSuXsZgURHn99201626;     ajamnfdfMcSuXsZgURHn99201626 = ajamnfdfMcSuXsZgURHn81601401;     ajamnfdfMcSuXsZgURHn81601401 = ajamnfdfMcSuXsZgURHn34189650;     ajamnfdfMcSuXsZgURHn34189650 = ajamnfdfMcSuXsZgURHn5515196;     ajamnfdfMcSuXsZgURHn5515196 = ajamnfdfMcSuXsZgURHn947369;     ajamnfdfMcSuXsZgURHn947369 = ajamnfdfMcSuXsZgURHn38232488;     ajamnfdfMcSuXsZgURHn38232488 = ajamnfdfMcSuXsZgURHn64399879;     ajamnfdfMcSuXsZgURHn64399879 = ajamnfdfMcSuXsZgURHn7893831;     ajamnfdfMcSuXsZgURHn7893831 = ajamnfdfMcSuXsZgURHn12998608;     ajamnfdfMcSuXsZgURHn12998608 = ajamnfdfMcSuXsZgURHn88208400;     ajamnfdfMcSuXsZgURHn88208400 = ajamnfdfMcSuXsZgURHn1336045;     ajamnfdfMcSuXsZgURHn1336045 = ajamnfdfMcSuXsZgURHn30590260;     ajamnfdfMcSuXsZgURHn30590260 = ajamnfdfMcSuXsZgURHn30138433;     ajamnfdfMcSuXsZgURHn30138433 = ajamnfdfMcSuXsZgURHn34115329;     ajamnfdfMcSuXsZgURHn34115329 = ajamnfdfMcSuXsZgURHn59619365;     ajamnfdfMcSuXsZgURHn59619365 = ajamnfdfMcSuXsZgURHn38580920;     ajamnfdfMcSuXsZgURHn38580920 = ajamnfdfMcSuXsZgURHn14924103;     ajamnfdfMcSuXsZgURHn14924103 = ajamnfdfMcSuXsZgURHn35048619;     ajamnfdfMcSuXsZgURHn35048619 = ajamnfdfMcSuXsZgURHn4313258;     ajamnfdfMcSuXsZgURHn4313258 = ajamnfdfMcSuXsZgURHn22474741;     ajamnfdfMcSuXsZgURHn22474741 = ajamnfdfMcSuXsZgURHn67379990;     ajamnfdfMcSuXsZgURHn67379990 = ajamnfdfMcSuXsZgURHn75547637;     ajamnfdfMcSuXsZgURHn75547637 = ajamnfdfMcSuXsZgURHn46852685;     ajamnfdfMcSuXsZgURHn46852685 = ajamnfdfMcSuXsZgURHn52060995;     ajamnfdfMcSuXsZgURHn52060995 = ajamnfdfMcSuXsZgURHn82885941;     ajamnfdfMcSuXsZgURHn82885941 = ajamnfdfMcSuXsZgURHn61232584;     ajamnfdfMcSuXsZgURHn61232584 = ajamnfdfMcSuXsZgURHn26632095;     ajamnfdfMcSuXsZgURHn26632095 = ajamnfdfMcSuXsZgURHn25431438;     ajamnfdfMcSuXsZgURHn25431438 = ajamnfdfMcSuXsZgURHn74837846;     ajamnfdfMcSuXsZgURHn74837846 = ajamnfdfMcSuXsZgURHn63887041;     ajamnfdfMcSuXsZgURHn63887041 = ajamnfdfMcSuXsZgURHn39703174;     ajamnfdfMcSuXsZgURHn39703174 = ajamnfdfMcSuXsZgURHn47607136;     ajamnfdfMcSuXsZgURHn47607136 = ajamnfdfMcSuXsZgURHn6501988;     ajamnfdfMcSuXsZgURHn6501988 = ajamnfdfMcSuXsZgURHn60205445;     ajamnfdfMcSuXsZgURHn60205445 = ajamnfdfMcSuXsZgURHn42326125;     ajamnfdfMcSuXsZgURHn42326125 = ajamnfdfMcSuXsZgURHn26982381;     ajamnfdfMcSuXsZgURHn26982381 = ajamnfdfMcSuXsZgURHn69267876;     ajamnfdfMcSuXsZgURHn69267876 = ajamnfdfMcSuXsZgURHn91994108;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void XqKifftQlNXqrznvgwHh55999127() {     float jMudTDJipdgXBvmXBnmX32115563 = 54853455;    float jMudTDJipdgXBvmXBnmX68416903 = -953626941;    float jMudTDJipdgXBvmXBnmX386839 = -892764230;    float jMudTDJipdgXBvmXBnmX13816403 = -454794819;    float jMudTDJipdgXBvmXBnmX46952766 = -821915827;    float jMudTDJipdgXBvmXBnmX6539553 = -848346193;    float jMudTDJipdgXBvmXBnmX68767958 = -480289165;    float jMudTDJipdgXBvmXBnmX73137867 = -573601144;    float jMudTDJipdgXBvmXBnmX7147548 = -368948890;    float jMudTDJipdgXBvmXBnmX37548580 = -628208874;    float jMudTDJipdgXBvmXBnmX14522561 = -23431532;    float jMudTDJipdgXBvmXBnmX78021326 = -242255931;    float jMudTDJipdgXBvmXBnmX99487912 = -522291208;    float jMudTDJipdgXBvmXBnmX85017628 = -172438597;    float jMudTDJipdgXBvmXBnmX84944225 = -191966340;    float jMudTDJipdgXBvmXBnmX24705614 = -581162511;    float jMudTDJipdgXBvmXBnmX26260327 = -476047566;    float jMudTDJipdgXBvmXBnmX41583511 = -3682597;    float jMudTDJipdgXBvmXBnmX63020585 = -19703247;    float jMudTDJipdgXBvmXBnmX40038250 = -74660771;    float jMudTDJipdgXBvmXBnmX88304739 = -133859664;    float jMudTDJipdgXBvmXBnmX7446863 = -641668750;    float jMudTDJipdgXBvmXBnmX29100363 = -488815674;    float jMudTDJipdgXBvmXBnmX32697125 = -681110446;    float jMudTDJipdgXBvmXBnmX81202209 = -159878479;    float jMudTDJipdgXBvmXBnmX83421653 = -993412358;    float jMudTDJipdgXBvmXBnmX86440783 = -444901549;    float jMudTDJipdgXBvmXBnmX65850605 = -433057809;    float jMudTDJipdgXBvmXBnmX88780208 = -830733790;    float jMudTDJipdgXBvmXBnmX1935593 = -366004924;    float jMudTDJipdgXBvmXBnmX82734785 = -297723602;    float jMudTDJipdgXBvmXBnmX76850444 = -235009501;    float jMudTDJipdgXBvmXBnmX50096224 = -267676486;    float jMudTDJipdgXBvmXBnmX10355500 = -411518981;    float jMudTDJipdgXBvmXBnmX90348184 = -323658737;    float jMudTDJipdgXBvmXBnmX23943041 = -55564482;    float jMudTDJipdgXBvmXBnmX45898225 = -204986801;    float jMudTDJipdgXBvmXBnmX54205512 = -937066732;    float jMudTDJipdgXBvmXBnmX76787212 = -146467326;    float jMudTDJipdgXBvmXBnmX83865758 = -974619057;    float jMudTDJipdgXBvmXBnmX42659844 = -742098323;    float jMudTDJipdgXBvmXBnmX54409545 = -778145917;    float jMudTDJipdgXBvmXBnmX71557113 = -582206233;    float jMudTDJipdgXBvmXBnmX58669646 = -902518904;    float jMudTDJipdgXBvmXBnmX8435484 = -154094146;    float jMudTDJipdgXBvmXBnmX23820871 = -826852568;    float jMudTDJipdgXBvmXBnmX93348146 = -617415165;    float jMudTDJipdgXBvmXBnmX4418150 = -490056892;    float jMudTDJipdgXBvmXBnmX10565292 = -562216567;    float jMudTDJipdgXBvmXBnmX94996858 = -717400606;    float jMudTDJipdgXBvmXBnmX85841524 = 13214959;    float jMudTDJipdgXBvmXBnmX12042739 = -125746662;    float jMudTDJipdgXBvmXBnmX17918360 = -150500014;    float jMudTDJipdgXBvmXBnmX54746689 = -782924865;    float jMudTDJipdgXBvmXBnmX67694388 = 12256702;    float jMudTDJipdgXBvmXBnmX24668701 = -303477796;    float jMudTDJipdgXBvmXBnmX39316541 = -364811267;    float jMudTDJipdgXBvmXBnmX67689714 = -111653785;    float jMudTDJipdgXBvmXBnmX32614193 = -194916340;    float jMudTDJipdgXBvmXBnmX63531112 = -828503469;    float jMudTDJipdgXBvmXBnmX20098769 = -303444645;    float jMudTDJipdgXBvmXBnmX2917354 = 52768643;    float jMudTDJipdgXBvmXBnmX84357659 = -742867355;    float jMudTDJipdgXBvmXBnmX5211955 = 97056033;    float jMudTDJipdgXBvmXBnmX54813795 = -230485273;    float jMudTDJipdgXBvmXBnmX37672116 = -788422032;    float jMudTDJipdgXBvmXBnmX27925103 = -974579445;    float jMudTDJipdgXBvmXBnmX89132413 = -10772227;    float jMudTDJipdgXBvmXBnmX94669443 = -848779861;    float jMudTDJipdgXBvmXBnmX61001184 = -36401858;    float jMudTDJipdgXBvmXBnmX78807388 = -276175711;    float jMudTDJipdgXBvmXBnmX72054815 = -538980834;    float jMudTDJipdgXBvmXBnmX64796299 = -857215271;    float jMudTDJipdgXBvmXBnmX79154827 = -45084190;    float jMudTDJipdgXBvmXBnmX97378406 = -332562449;    float jMudTDJipdgXBvmXBnmX33895195 = -355713748;    float jMudTDJipdgXBvmXBnmX35889749 = 40537482;    float jMudTDJipdgXBvmXBnmX70430716 = -586296771;    float jMudTDJipdgXBvmXBnmX24261642 = -427016300;    float jMudTDJipdgXBvmXBnmX57381338 = -333025912;    float jMudTDJipdgXBvmXBnmX90073507 = -275997194;    float jMudTDJipdgXBvmXBnmX82022633 = -954844657;    float jMudTDJipdgXBvmXBnmX55285314 = -870841242;    float jMudTDJipdgXBvmXBnmX93783349 = -13333184;    float jMudTDJipdgXBvmXBnmX16094069 = -279219884;    float jMudTDJipdgXBvmXBnmX70692046 = -71976940;    float jMudTDJipdgXBvmXBnmX58932085 = 15490513;    float jMudTDJipdgXBvmXBnmX95349534 = -484751622;    float jMudTDJipdgXBvmXBnmX42661111 = -323775683;    float jMudTDJipdgXBvmXBnmX65679484 = 79819058;    float jMudTDJipdgXBvmXBnmX84626500 = -690753215;    float jMudTDJipdgXBvmXBnmX78208511 = 6666984;    float jMudTDJipdgXBvmXBnmX21591320 = -642150392;    float jMudTDJipdgXBvmXBnmX13256100 = -317963857;    float jMudTDJipdgXBvmXBnmX63766989 = -571174413;    float jMudTDJipdgXBvmXBnmX39742490 = -694866966;    float jMudTDJipdgXBvmXBnmX70051885 = 64721437;    float jMudTDJipdgXBvmXBnmX66345158 = -579262267;    float jMudTDJipdgXBvmXBnmX3855852 = -572033632;    float jMudTDJipdgXBvmXBnmX70763367 = 54853455;     jMudTDJipdgXBvmXBnmX32115563 = jMudTDJipdgXBvmXBnmX68416903;     jMudTDJipdgXBvmXBnmX68416903 = jMudTDJipdgXBvmXBnmX386839;     jMudTDJipdgXBvmXBnmX386839 = jMudTDJipdgXBvmXBnmX13816403;     jMudTDJipdgXBvmXBnmX13816403 = jMudTDJipdgXBvmXBnmX46952766;     jMudTDJipdgXBvmXBnmX46952766 = jMudTDJipdgXBvmXBnmX6539553;     jMudTDJipdgXBvmXBnmX6539553 = jMudTDJipdgXBvmXBnmX68767958;     jMudTDJipdgXBvmXBnmX68767958 = jMudTDJipdgXBvmXBnmX73137867;     jMudTDJipdgXBvmXBnmX73137867 = jMudTDJipdgXBvmXBnmX7147548;     jMudTDJipdgXBvmXBnmX7147548 = jMudTDJipdgXBvmXBnmX37548580;     jMudTDJipdgXBvmXBnmX37548580 = jMudTDJipdgXBvmXBnmX14522561;     jMudTDJipdgXBvmXBnmX14522561 = jMudTDJipdgXBvmXBnmX78021326;     jMudTDJipdgXBvmXBnmX78021326 = jMudTDJipdgXBvmXBnmX99487912;     jMudTDJipdgXBvmXBnmX99487912 = jMudTDJipdgXBvmXBnmX85017628;     jMudTDJipdgXBvmXBnmX85017628 = jMudTDJipdgXBvmXBnmX84944225;     jMudTDJipdgXBvmXBnmX84944225 = jMudTDJipdgXBvmXBnmX24705614;     jMudTDJipdgXBvmXBnmX24705614 = jMudTDJipdgXBvmXBnmX26260327;     jMudTDJipdgXBvmXBnmX26260327 = jMudTDJipdgXBvmXBnmX41583511;     jMudTDJipdgXBvmXBnmX41583511 = jMudTDJipdgXBvmXBnmX63020585;     jMudTDJipdgXBvmXBnmX63020585 = jMudTDJipdgXBvmXBnmX40038250;     jMudTDJipdgXBvmXBnmX40038250 = jMudTDJipdgXBvmXBnmX88304739;     jMudTDJipdgXBvmXBnmX88304739 = jMudTDJipdgXBvmXBnmX7446863;     jMudTDJipdgXBvmXBnmX7446863 = jMudTDJipdgXBvmXBnmX29100363;     jMudTDJipdgXBvmXBnmX29100363 = jMudTDJipdgXBvmXBnmX32697125;     jMudTDJipdgXBvmXBnmX32697125 = jMudTDJipdgXBvmXBnmX81202209;     jMudTDJipdgXBvmXBnmX81202209 = jMudTDJipdgXBvmXBnmX83421653;     jMudTDJipdgXBvmXBnmX83421653 = jMudTDJipdgXBvmXBnmX86440783;     jMudTDJipdgXBvmXBnmX86440783 = jMudTDJipdgXBvmXBnmX65850605;     jMudTDJipdgXBvmXBnmX65850605 = jMudTDJipdgXBvmXBnmX88780208;     jMudTDJipdgXBvmXBnmX88780208 = jMudTDJipdgXBvmXBnmX1935593;     jMudTDJipdgXBvmXBnmX1935593 = jMudTDJipdgXBvmXBnmX82734785;     jMudTDJipdgXBvmXBnmX82734785 = jMudTDJipdgXBvmXBnmX76850444;     jMudTDJipdgXBvmXBnmX76850444 = jMudTDJipdgXBvmXBnmX50096224;     jMudTDJipdgXBvmXBnmX50096224 = jMudTDJipdgXBvmXBnmX10355500;     jMudTDJipdgXBvmXBnmX10355500 = jMudTDJipdgXBvmXBnmX90348184;     jMudTDJipdgXBvmXBnmX90348184 = jMudTDJipdgXBvmXBnmX23943041;     jMudTDJipdgXBvmXBnmX23943041 = jMudTDJipdgXBvmXBnmX45898225;     jMudTDJipdgXBvmXBnmX45898225 = jMudTDJipdgXBvmXBnmX54205512;     jMudTDJipdgXBvmXBnmX54205512 = jMudTDJipdgXBvmXBnmX76787212;     jMudTDJipdgXBvmXBnmX76787212 = jMudTDJipdgXBvmXBnmX83865758;     jMudTDJipdgXBvmXBnmX83865758 = jMudTDJipdgXBvmXBnmX42659844;     jMudTDJipdgXBvmXBnmX42659844 = jMudTDJipdgXBvmXBnmX54409545;     jMudTDJipdgXBvmXBnmX54409545 = jMudTDJipdgXBvmXBnmX71557113;     jMudTDJipdgXBvmXBnmX71557113 = jMudTDJipdgXBvmXBnmX58669646;     jMudTDJipdgXBvmXBnmX58669646 = jMudTDJipdgXBvmXBnmX8435484;     jMudTDJipdgXBvmXBnmX8435484 = jMudTDJipdgXBvmXBnmX23820871;     jMudTDJipdgXBvmXBnmX23820871 = jMudTDJipdgXBvmXBnmX93348146;     jMudTDJipdgXBvmXBnmX93348146 = jMudTDJipdgXBvmXBnmX4418150;     jMudTDJipdgXBvmXBnmX4418150 = jMudTDJipdgXBvmXBnmX10565292;     jMudTDJipdgXBvmXBnmX10565292 = jMudTDJipdgXBvmXBnmX94996858;     jMudTDJipdgXBvmXBnmX94996858 = jMudTDJipdgXBvmXBnmX85841524;     jMudTDJipdgXBvmXBnmX85841524 = jMudTDJipdgXBvmXBnmX12042739;     jMudTDJipdgXBvmXBnmX12042739 = jMudTDJipdgXBvmXBnmX17918360;     jMudTDJipdgXBvmXBnmX17918360 = jMudTDJipdgXBvmXBnmX54746689;     jMudTDJipdgXBvmXBnmX54746689 = jMudTDJipdgXBvmXBnmX67694388;     jMudTDJipdgXBvmXBnmX67694388 = jMudTDJipdgXBvmXBnmX24668701;     jMudTDJipdgXBvmXBnmX24668701 = jMudTDJipdgXBvmXBnmX39316541;     jMudTDJipdgXBvmXBnmX39316541 = jMudTDJipdgXBvmXBnmX67689714;     jMudTDJipdgXBvmXBnmX67689714 = jMudTDJipdgXBvmXBnmX32614193;     jMudTDJipdgXBvmXBnmX32614193 = jMudTDJipdgXBvmXBnmX63531112;     jMudTDJipdgXBvmXBnmX63531112 = jMudTDJipdgXBvmXBnmX20098769;     jMudTDJipdgXBvmXBnmX20098769 = jMudTDJipdgXBvmXBnmX2917354;     jMudTDJipdgXBvmXBnmX2917354 = jMudTDJipdgXBvmXBnmX84357659;     jMudTDJipdgXBvmXBnmX84357659 = jMudTDJipdgXBvmXBnmX5211955;     jMudTDJipdgXBvmXBnmX5211955 = jMudTDJipdgXBvmXBnmX54813795;     jMudTDJipdgXBvmXBnmX54813795 = jMudTDJipdgXBvmXBnmX37672116;     jMudTDJipdgXBvmXBnmX37672116 = jMudTDJipdgXBvmXBnmX27925103;     jMudTDJipdgXBvmXBnmX27925103 = jMudTDJipdgXBvmXBnmX89132413;     jMudTDJipdgXBvmXBnmX89132413 = jMudTDJipdgXBvmXBnmX94669443;     jMudTDJipdgXBvmXBnmX94669443 = jMudTDJipdgXBvmXBnmX61001184;     jMudTDJipdgXBvmXBnmX61001184 = jMudTDJipdgXBvmXBnmX78807388;     jMudTDJipdgXBvmXBnmX78807388 = jMudTDJipdgXBvmXBnmX72054815;     jMudTDJipdgXBvmXBnmX72054815 = jMudTDJipdgXBvmXBnmX64796299;     jMudTDJipdgXBvmXBnmX64796299 = jMudTDJipdgXBvmXBnmX79154827;     jMudTDJipdgXBvmXBnmX79154827 = jMudTDJipdgXBvmXBnmX97378406;     jMudTDJipdgXBvmXBnmX97378406 = jMudTDJipdgXBvmXBnmX33895195;     jMudTDJipdgXBvmXBnmX33895195 = jMudTDJipdgXBvmXBnmX35889749;     jMudTDJipdgXBvmXBnmX35889749 = jMudTDJipdgXBvmXBnmX70430716;     jMudTDJipdgXBvmXBnmX70430716 = jMudTDJipdgXBvmXBnmX24261642;     jMudTDJipdgXBvmXBnmX24261642 = jMudTDJipdgXBvmXBnmX57381338;     jMudTDJipdgXBvmXBnmX57381338 = jMudTDJipdgXBvmXBnmX90073507;     jMudTDJipdgXBvmXBnmX90073507 = jMudTDJipdgXBvmXBnmX82022633;     jMudTDJipdgXBvmXBnmX82022633 = jMudTDJipdgXBvmXBnmX55285314;     jMudTDJipdgXBvmXBnmX55285314 = jMudTDJipdgXBvmXBnmX93783349;     jMudTDJipdgXBvmXBnmX93783349 = jMudTDJipdgXBvmXBnmX16094069;     jMudTDJipdgXBvmXBnmX16094069 = jMudTDJipdgXBvmXBnmX70692046;     jMudTDJipdgXBvmXBnmX70692046 = jMudTDJipdgXBvmXBnmX58932085;     jMudTDJipdgXBvmXBnmX58932085 = jMudTDJipdgXBvmXBnmX95349534;     jMudTDJipdgXBvmXBnmX95349534 = jMudTDJipdgXBvmXBnmX42661111;     jMudTDJipdgXBvmXBnmX42661111 = jMudTDJipdgXBvmXBnmX65679484;     jMudTDJipdgXBvmXBnmX65679484 = jMudTDJipdgXBvmXBnmX84626500;     jMudTDJipdgXBvmXBnmX84626500 = jMudTDJipdgXBvmXBnmX78208511;     jMudTDJipdgXBvmXBnmX78208511 = jMudTDJipdgXBvmXBnmX21591320;     jMudTDJipdgXBvmXBnmX21591320 = jMudTDJipdgXBvmXBnmX13256100;     jMudTDJipdgXBvmXBnmX13256100 = jMudTDJipdgXBvmXBnmX63766989;     jMudTDJipdgXBvmXBnmX63766989 = jMudTDJipdgXBvmXBnmX39742490;     jMudTDJipdgXBvmXBnmX39742490 = jMudTDJipdgXBvmXBnmX70051885;     jMudTDJipdgXBvmXBnmX70051885 = jMudTDJipdgXBvmXBnmX66345158;     jMudTDJipdgXBvmXBnmX66345158 = jMudTDJipdgXBvmXBnmX3855852;     jMudTDJipdgXBvmXBnmX3855852 = jMudTDJipdgXBvmXBnmX70763367;     jMudTDJipdgXBvmXBnmX70763367 = jMudTDJipdgXBvmXBnmX32115563;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void IVmAaStqLbAVBrBbXcnI10415341() {     float jNYhqFuWmfaLBPRdaBoH2767231 = -563295676;    float jNYhqFuWmfaLBPRdaBoH37090421 = -825270240;    float jNYhqFuWmfaLBPRdaBoH80355757 = -235058690;    float jNYhqFuWmfaLBPRdaBoH53747689 = -644933208;    float jNYhqFuWmfaLBPRdaBoH68403087 = -605322174;    float jNYhqFuWmfaLBPRdaBoH96539315 = -549927344;    float jNYhqFuWmfaLBPRdaBoH87146854 = -479064875;    float jNYhqFuWmfaLBPRdaBoH29980248 = -920418234;    float jNYhqFuWmfaLBPRdaBoH53012602 = -462296219;    float jNYhqFuWmfaLBPRdaBoH60515041 = -929304225;    float jNYhqFuWmfaLBPRdaBoH56771721 = -58333462;    float jNYhqFuWmfaLBPRdaBoH23567898 = -558019724;    float jNYhqFuWmfaLBPRdaBoH79315299 = -431828266;    float jNYhqFuWmfaLBPRdaBoH68864662 = 26533193;    float jNYhqFuWmfaLBPRdaBoH67664244 = -341716343;    float jNYhqFuWmfaLBPRdaBoH57234254 = -151146098;    float jNYhqFuWmfaLBPRdaBoH20113154 = -385232696;    float jNYhqFuWmfaLBPRdaBoH61877005 = 7151207;    float jNYhqFuWmfaLBPRdaBoH44728367 = -726891189;    float jNYhqFuWmfaLBPRdaBoH98824959 = -617027076;    float jNYhqFuWmfaLBPRdaBoH43440494 = -42538805;    float jNYhqFuWmfaLBPRdaBoH68842627 = -972954056;    float jNYhqFuWmfaLBPRdaBoH31044171 = 43341181;    float jNYhqFuWmfaLBPRdaBoH66546662 = -273511352;    float jNYhqFuWmfaLBPRdaBoH45326598 = -935783166;    float jNYhqFuWmfaLBPRdaBoH14019234 = 8303197;    float jNYhqFuWmfaLBPRdaBoH94701569 = -840079324;    float jNYhqFuWmfaLBPRdaBoH73632830 = -938181576;    float jNYhqFuWmfaLBPRdaBoH20922697 = -25092006;    float jNYhqFuWmfaLBPRdaBoH41107689 = -924399994;    float jNYhqFuWmfaLBPRdaBoH89806781 = -46157287;    float jNYhqFuWmfaLBPRdaBoH21361906 = -563306663;    float jNYhqFuWmfaLBPRdaBoH33041585 = -214638095;    float jNYhqFuWmfaLBPRdaBoH75720474 = 24951335;    float jNYhqFuWmfaLBPRdaBoH68372421 = -170605517;    float jNYhqFuWmfaLBPRdaBoH83936300 = -134791030;    float jNYhqFuWmfaLBPRdaBoH89689711 = -393580008;    float jNYhqFuWmfaLBPRdaBoH6998001 = -721179840;    float jNYhqFuWmfaLBPRdaBoH55320501 = -102195963;    float jNYhqFuWmfaLBPRdaBoH78969056 = -492227420;    float jNYhqFuWmfaLBPRdaBoH2733128 = -26117201;    float jNYhqFuWmfaLBPRdaBoH82892536 = -547534987;    float jNYhqFuWmfaLBPRdaBoH20524074 = -568960285;    float jNYhqFuWmfaLBPRdaBoH65177947 = -974493876;    float jNYhqFuWmfaLBPRdaBoH4389542 = -462740541;    float jNYhqFuWmfaLBPRdaBoH30226706 = -585189723;    float jNYhqFuWmfaLBPRdaBoH19479350 = -733484964;    float jNYhqFuWmfaLBPRdaBoH70032360 = -942187187;    float jNYhqFuWmfaLBPRdaBoH16006744 = -547980536;    float jNYhqFuWmfaLBPRdaBoH3424345 = 99590516;    float jNYhqFuWmfaLBPRdaBoH54030849 = -633357162;    float jNYhqFuWmfaLBPRdaBoH53914243 = -94966384;    float jNYhqFuWmfaLBPRdaBoH72994016 = -78298056;    float jNYhqFuWmfaLBPRdaBoH44973994 = -343806362;    float jNYhqFuWmfaLBPRdaBoH66974847 = -98684213;    float jNYhqFuWmfaLBPRdaBoH33924603 = -590341620;    float jNYhqFuWmfaLBPRdaBoH6046250 = -768611422;    float jNYhqFuWmfaLBPRdaBoH13809096 = -961547338;    float jNYhqFuWmfaLBPRdaBoH8421092 = -709150042;    float jNYhqFuWmfaLBPRdaBoH54383853 = -513625372;    float jNYhqFuWmfaLBPRdaBoH1837746 = -709848021;    float jNYhqFuWmfaLBPRdaBoH13514024 = -540883299;    float jNYhqFuWmfaLBPRdaBoH9057552 = -795326228;    float jNYhqFuWmfaLBPRdaBoH11904914 = -537896225;    float jNYhqFuWmfaLBPRdaBoH70708259 = -783146938;    float jNYhqFuWmfaLBPRdaBoH35409816 = -495026799;    float jNYhqFuWmfaLBPRdaBoH90526313 = -243381629;    float jNYhqFuWmfaLBPRdaBoH3594826 = -356779601;    float jNYhqFuWmfaLBPRdaBoH492241 = -802861291;    float jNYhqFuWmfaLBPRdaBoH83727944 = -106925314;    float jNYhqFuWmfaLBPRdaBoH67544542 = -757566090;    float jNYhqFuWmfaLBPRdaBoH13115153 = -664052857;    float jNYhqFuWmfaLBPRdaBoH6556505 = -890652831;    float jNYhqFuWmfaLBPRdaBoH65759310 = -134663769;    float jNYhqFuWmfaLBPRdaBoH96091831 = -490909875;    float jNYhqFuWmfaLBPRdaBoH60547958 = -495003819;    float jNYhqFuWmfaLBPRdaBoH48318554 = -303993771;    float jNYhqFuWmfaLBPRdaBoH65866224 = 17835056;    float jNYhqFuWmfaLBPRdaBoH62157120 = -810770812;    float jNYhqFuWmfaLBPRdaBoH15099892 = -250593444;    float jNYhqFuWmfaLBPRdaBoH94539884 = -258211839;    float jNYhqFuWmfaLBPRdaBoH24669210 = -897892137;    float jNYhqFuWmfaLBPRdaBoH57626087 = -290201040;    float jNYhqFuWmfaLBPRdaBoH17498352 = -24682523;    float jNYhqFuWmfaLBPRdaBoH87076839 = -191042833;    float jNYhqFuWmfaLBPRdaBoH35892539 = -951190904;    float jNYhqFuWmfaLBPRdaBoH48367889 = -385008608;    float jNYhqFuWmfaLBPRdaBoH88067590 = -870831734;    float jNYhqFuWmfaLBPRdaBoH8745628 = -876364453;    float jNYhqFuWmfaLBPRdaBoH34447818 = -580263897;    float jNYhqFuWmfaLBPRdaBoH77890050 = -366179609;    float jNYhqFuWmfaLBPRdaBoH75880615 = -432032670;    float jNYhqFuWmfaLBPRdaBoH98576909 = 87970202;    float jNYhqFuWmfaLBPRdaBoH936648 = -588570591;    float jNYhqFuWmfaLBPRdaBoH77131311 = -782379399;    float jNYhqFuWmfaLBPRdaBoH89219104 = -485233902;    float jNYhqFuWmfaLBPRdaBoH73834984 = -752208759;    float jNYhqFuWmfaLBPRdaBoH8619160 = 68935939;    float jNYhqFuWmfaLBPRdaBoH94469687 = -91346938;    float jNYhqFuWmfaLBPRdaBoH68979726 = -563295676;     jNYhqFuWmfaLBPRdaBoH2767231 = jNYhqFuWmfaLBPRdaBoH37090421;     jNYhqFuWmfaLBPRdaBoH37090421 = jNYhqFuWmfaLBPRdaBoH80355757;     jNYhqFuWmfaLBPRdaBoH80355757 = jNYhqFuWmfaLBPRdaBoH53747689;     jNYhqFuWmfaLBPRdaBoH53747689 = jNYhqFuWmfaLBPRdaBoH68403087;     jNYhqFuWmfaLBPRdaBoH68403087 = jNYhqFuWmfaLBPRdaBoH96539315;     jNYhqFuWmfaLBPRdaBoH96539315 = jNYhqFuWmfaLBPRdaBoH87146854;     jNYhqFuWmfaLBPRdaBoH87146854 = jNYhqFuWmfaLBPRdaBoH29980248;     jNYhqFuWmfaLBPRdaBoH29980248 = jNYhqFuWmfaLBPRdaBoH53012602;     jNYhqFuWmfaLBPRdaBoH53012602 = jNYhqFuWmfaLBPRdaBoH60515041;     jNYhqFuWmfaLBPRdaBoH60515041 = jNYhqFuWmfaLBPRdaBoH56771721;     jNYhqFuWmfaLBPRdaBoH56771721 = jNYhqFuWmfaLBPRdaBoH23567898;     jNYhqFuWmfaLBPRdaBoH23567898 = jNYhqFuWmfaLBPRdaBoH79315299;     jNYhqFuWmfaLBPRdaBoH79315299 = jNYhqFuWmfaLBPRdaBoH68864662;     jNYhqFuWmfaLBPRdaBoH68864662 = jNYhqFuWmfaLBPRdaBoH67664244;     jNYhqFuWmfaLBPRdaBoH67664244 = jNYhqFuWmfaLBPRdaBoH57234254;     jNYhqFuWmfaLBPRdaBoH57234254 = jNYhqFuWmfaLBPRdaBoH20113154;     jNYhqFuWmfaLBPRdaBoH20113154 = jNYhqFuWmfaLBPRdaBoH61877005;     jNYhqFuWmfaLBPRdaBoH61877005 = jNYhqFuWmfaLBPRdaBoH44728367;     jNYhqFuWmfaLBPRdaBoH44728367 = jNYhqFuWmfaLBPRdaBoH98824959;     jNYhqFuWmfaLBPRdaBoH98824959 = jNYhqFuWmfaLBPRdaBoH43440494;     jNYhqFuWmfaLBPRdaBoH43440494 = jNYhqFuWmfaLBPRdaBoH68842627;     jNYhqFuWmfaLBPRdaBoH68842627 = jNYhqFuWmfaLBPRdaBoH31044171;     jNYhqFuWmfaLBPRdaBoH31044171 = jNYhqFuWmfaLBPRdaBoH66546662;     jNYhqFuWmfaLBPRdaBoH66546662 = jNYhqFuWmfaLBPRdaBoH45326598;     jNYhqFuWmfaLBPRdaBoH45326598 = jNYhqFuWmfaLBPRdaBoH14019234;     jNYhqFuWmfaLBPRdaBoH14019234 = jNYhqFuWmfaLBPRdaBoH94701569;     jNYhqFuWmfaLBPRdaBoH94701569 = jNYhqFuWmfaLBPRdaBoH73632830;     jNYhqFuWmfaLBPRdaBoH73632830 = jNYhqFuWmfaLBPRdaBoH20922697;     jNYhqFuWmfaLBPRdaBoH20922697 = jNYhqFuWmfaLBPRdaBoH41107689;     jNYhqFuWmfaLBPRdaBoH41107689 = jNYhqFuWmfaLBPRdaBoH89806781;     jNYhqFuWmfaLBPRdaBoH89806781 = jNYhqFuWmfaLBPRdaBoH21361906;     jNYhqFuWmfaLBPRdaBoH21361906 = jNYhqFuWmfaLBPRdaBoH33041585;     jNYhqFuWmfaLBPRdaBoH33041585 = jNYhqFuWmfaLBPRdaBoH75720474;     jNYhqFuWmfaLBPRdaBoH75720474 = jNYhqFuWmfaLBPRdaBoH68372421;     jNYhqFuWmfaLBPRdaBoH68372421 = jNYhqFuWmfaLBPRdaBoH83936300;     jNYhqFuWmfaLBPRdaBoH83936300 = jNYhqFuWmfaLBPRdaBoH89689711;     jNYhqFuWmfaLBPRdaBoH89689711 = jNYhqFuWmfaLBPRdaBoH6998001;     jNYhqFuWmfaLBPRdaBoH6998001 = jNYhqFuWmfaLBPRdaBoH55320501;     jNYhqFuWmfaLBPRdaBoH55320501 = jNYhqFuWmfaLBPRdaBoH78969056;     jNYhqFuWmfaLBPRdaBoH78969056 = jNYhqFuWmfaLBPRdaBoH2733128;     jNYhqFuWmfaLBPRdaBoH2733128 = jNYhqFuWmfaLBPRdaBoH82892536;     jNYhqFuWmfaLBPRdaBoH82892536 = jNYhqFuWmfaLBPRdaBoH20524074;     jNYhqFuWmfaLBPRdaBoH20524074 = jNYhqFuWmfaLBPRdaBoH65177947;     jNYhqFuWmfaLBPRdaBoH65177947 = jNYhqFuWmfaLBPRdaBoH4389542;     jNYhqFuWmfaLBPRdaBoH4389542 = jNYhqFuWmfaLBPRdaBoH30226706;     jNYhqFuWmfaLBPRdaBoH30226706 = jNYhqFuWmfaLBPRdaBoH19479350;     jNYhqFuWmfaLBPRdaBoH19479350 = jNYhqFuWmfaLBPRdaBoH70032360;     jNYhqFuWmfaLBPRdaBoH70032360 = jNYhqFuWmfaLBPRdaBoH16006744;     jNYhqFuWmfaLBPRdaBoH16006744 = jNYhqFuWmfaLBPRdaBoH3424345;     jNYhqFuWmfaLBPRdaBoH3424345 = jNYhqFuWmfaLBPRdaBoH54030849;     jNYhqFuWmfaLBPRdaBoH54030849 = jNYhqFuWmfaLBPRdaBoH53914243;     jNYhqFuWmfaLBPRdaBoH53914243 = jNYhqFuWmfaLBPRdaBoH72994016;     jNYhqFuWmfaLBPRdaBoH72994016 = jNYhqFuWmfaLBPRdaBoH44973994;     jNYhqFuWmfaLBPRdaBoH44973994 = jNYhqFuWmfaLBPRdaBoH66974847;     jNYhqFuWmfaLBPRdaBoH66974847 = jNYhqFuWmfaLBPRdaBoH33924603;     jNYhqFuWmfaLBPRdaBoH33924603 = jNYhqFuWmfaLBPRdaBoH6046250;     jNYhqFuWmfaLBPRdaBoH6046250 = jNYhqFuWmfaLBPRdaBoH13809096;     jNYhqFuWmfaLBPRdaBoH13809096 = jNYhqFuWmfaLBPRdaBoH8421092;     jNYhqFuWmfaLBPRdaBoH8421092 = jNYhqFuWmfaLBPRdaBoH54383853;     jNYhqFuWmfaLBPRdaBoH54383853 = jNYhqFuWmfaLBPRdaBoH1837746;     jNYhqFuWmfaLBPRdaBoH1837746 = jNYhqFuWmfaLBPRdaBoH13514024;     jNYhqFuWmfaLBPRdaBoH13514024 = jNYhqFuWmfaLBPRdaBoH9057552;     jNYhqFuWmfaLBPRdaBoH9057552 = jNYhqFuWmfaLBPRdaBoH11904914;     jNYhqFuWmfaLBPRdaBoH11904914 = jNYhqFuWmfaLBPRdaBoH70708259;     jNYhqFuWmfaLBPRdaBoH70708259 = jNYhqFuWmfaLBPRdaBoH35409816;     jNYhqFuWmfaLBPRdaBoH35409816 = jNYhqFuWmfaLBPRdaBoH90526313;     jNYhqFuWmfaLBPRdaBoH90526313 = jNYhqFuWmfaLBPRdaBoH3594826;     jNYhqFuWmfaLBPRdaBoH3594826 = jNYhqFuWmfaLBPRdaBoH492241;     jNYhqFuWmfaLBPRdaBoH492241 = jNYhqFuWmfaLBPRdaBoH83727944;     jNYhqFuWmfaLBPRdaBoH83727944 = jNYhqFuWmfaLBPRdaBoH67544542;     jNYhqFuWmfaLBPRdaBoH67544542 = jNYhqFuWmfaLBPRdaBoH13115153;     jNYhqFuWmfaLBPRdaBoH13115153 = jNYhqFuWmfaLBPRdaBoH6556505;     jNYhqFuWmfaLBPRdaBoH6556505 = jNYhqFuWmfaLBPRdaBoH65759310;     jNYhqFuWmfaLBPRdaBoH65759310 = jNYhqFuWmfaLBPRdaBoH96091831;     jNYhqFuWmfaLBPRdaBoH96091831 = jNYhqFuWmfaLBPRdaBoH60547958;     jNYhqFuWmfaLBPRdaBoH60547958 = jNYhqFuWmfaLBPRdaBoH48318554;     jNYhqFuWmfaLBPRdaBoH48318554 = jNYhqFuWmfaLBPRdaBoH65866224;     jNYhqFuWmfaLBPRdaBoH65866224 = jNYhqFuWmfaLBPRdaBoH62157120;     jNYhqFuWmfaLBPRdaBoH62157120 = jNYhqFuWmfaLBPRdaBoH15099892;     jNYhqFuWmfaLBPRdaBoH15099892 = jNYhqFuWmfaLBPRdaBoH94539884;     jNYhqFuWmfaLBPRdaBoH94539884 = jNYhqFuWmfaLBPRdaBoH24669210;     jNYhqFuWmfaLBPRdaBoH24669210 = jNYhqFuWmfaLBPRdaBoH57626087;     jNYhqFuWmfaLBPRdaBoH57626087 = jNYhqFuWmfaLBPRdaBoH17498352;     jNYhqFuWmfaLBPRdaBoH17498352 = jNYhqFuWmfaLBPRdaBoH87076839;     jNYhqFuWmfaLBPRdaBoH87076839 = jNYhqFuWmfaLBPRdaBoH35892539;     jNYhqFuWmfaLBPRdaBoH35892539 = jNYhqFuWmfaLBPRdaBoH48367889;     jNYhqFuWmfaLBPRdaBoH48367889 = jNYhqFuWmfaLBPRdaBoH88067590;     jNYhqFuWmfaLBPRdaBoH88067590 = jNYhqFuWmfaLBPRdaBoH8745628;     jNYhqFuWmfaLBPRdaBoH8745628 = jNYhqFuWmfaLBPRdaBoH34447818;     jNYhqFuWmfaLBPRdaBoH34447818 = jNYhqFuWmfaLBPRdaBoH77890050;     jNYhqFuWmfaLBPRdaBoH77890050 = jNYhqFuWmfaLBPRdaBoH75880615;     jNYhqFuWmfaLBPRdaBoH75880615 = jNYhqFuWmfaLBPRdaBoH98576909;     jNYhqFuWmfaLBPRdaBoH98576909 = jNYhqFuWmfaLBPRdaBoH936648;     jNYhqFuWmfaLBPRdaBoH936648 = jNYhqFuWmfaLBPRdaBoH77131311;     jNYhqFuWmfaLBPRdaBoH77131311 = jNYhqFuWmfaLBPRdaBoH89219104;     jNYhqFuWmfaLBPRdaBoH89219104 = jNYhqFuWmfaLBPRdaBoH73834984;     jNYhqFuWmfaLBPRdaBoH73834984 = jNYhqFuWmfaLBPRdaBoH8619160;     jNYhqFuWmfaLBPRdaBoH8619160 = jNYhqFuWmfaLBPRdaBoH94469687;     jNYhqFuWmfaLBPRdaBoH94469687 = jNYhqFuWmfaLBPRdaBoH68979726;     jNYhqFuWmfaLBPRdaBoH68979726 = jNYhqFuWmfaLBPRdaBoH2767231;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void HKJHHAoWFXxZImSIJdCP17074086() {     float lTnJGczmWrgyXqpYhThd2760759 = -435819817;    float lTnJGczmWrgyXqpYhThd75662737 = -867865365;    float lTnJGczmWrgyXqpYhThd81668969 = -192187303;    float lTnJGczmWrgyXqpYhThd38827550 = -465474038;    float lTnJGczmWrgyXqpYhThd77243189 = 26322504;    float lTnJGczmWrgyXqpYhThd31344748 = -249360156;    float lTnJGczmWrgyXqpYhThd55694190 = -183601342;    float lTnJGczmWrgyXqpYhThd99194298 = -624595551;    float lTnJGczmWrgyXqpYhThd35752767 = -175942289;    float lTnJGczmWrgyXqpYhThd57154554 = -566111096;    float lTnJGczmWrgyXqpYhThd77092326 = -17805534;    float lTnJGczmWrgyXqpYhThd37859575 = -293498087;    float lTnJGczmWrgyXqpYhThd11168214 = -453486740;    float lTnJGczmWrgyXqpYhThd22708882 = -318619859;    float lTnJGczmWrgyXqpYhThd12836354 = -304429195;    float lTnJGczmWrgyXqpYhThd31798801 = -483941749;    float lTnJGczmWrgyXqpYhThd75965913 = -204847266;    float lTnJGczmWrgyXqpYhThd84877538 = -389920415;    float lTnJGczmWrgyXqpYhThd65156637 = -244873455;    float lTnJGczmWrgyXqpYhThd70898722 = -129051527;    float lTnJGczmWrgyXqpYhThd23109918 = -853005178;    float lTnJGczmWrgyXqpYhThd47138893 = -382945939;    float lTnJGczmWrgyXqpYhThd67922567 = -415616661;    float lTnJGczmWrgyXqpYhThd10196770 = -154431874;    float lTnJGczmWrgyXqpYhThd73769287 = -262477020;    float lTnJGczmWrgyXqpYhThd30593311 = -999902731;    float lTnJGczmWrgyXqpYhThd64923151 = -40879037;    float lTnJGczmWrgyXqpYhThd9036356 = -447148564;    float lTnJGczmWrgyXqpYhThd33601143 = -671162321;    float lTnJGczmWrgyXqpYhThd17197559 = -599831368;    float lTnJGczmWrgyXqpYhThd23219461 = -433250522;    float lTnJGczmWrgyXqpYhThd6085763 = -955128425;    float lTnJGczmWrgyXqpYhThd18494008 = -416582311;    float lTnJGczmWrgyXqpYhThd76003311 = -951730398;    float lTnJGczmWrgyXqpYhThd50730459 = -429919441;    float lTnJGczmWrgyXqpYhThd26764490 = -214556776;    float lTnJGczmWrgyXqpYhThd1631079 = -493233386;    float lTnJGczmWrgyXqpYhThd76915680 = -716442519;    float lTnJGczmWrgyXqpYhThd90634097 = -814877379;    float lTnJGczmWrgyXqpYhThd50841950 = -101659106;    float lTnJGczmWrgyXqpYhThd90588795 = -114061099;    float lTnJGczmWrgyXqpYhThd68590197 = -741327069;    float lTnJGczmWrgyXqpYhThd80807433 = -28090539;    float lTnJGczmWrgyXqpYhThd24409845 = -531458319;    float lTnJGczmWrgyXqpYhThd88407594 = -935999942;    float lTnJGczmWrgyXqpYhThd70654674 = -397234393;    float lTnJGczmWrgyXqpYhThd47177142 = -114765586;    float lTnJGczmWrgyXqpYhThd45415941 = -450948543;    float lTnJGczmWrgyXqpYhThd34665133 = -111744398;    float lTnJGczmWrgyXqpYhThd23469768 = -587434371;    float lTnJGczmWrgyXqpYhThd67290219 = -211912169;    float lTnJGczmWrgyXqpYhThd68461500 = -477112132;    float lTnJGczmWrgyXqpYhThd48442365 = 29647936;    float lTnJGczmWrgyXqpYhThd55224265 = -875726213;    float lTnJGczmWrgyXqpYhThd93964167 = -955583490;    float lTnJGczmWrgyXqpYhThd55621866 = 47126121;    float lTnJGczmWrgyXqpYhThd7740170 = -352248705;    float lTnJGczmWrgyXqpYhThd71472199 = 62244571;    float lTnJGczmWrgyXqpYhThd65058262 = -102997019;    float lTnJGczmWrgyXqpYhThd46649879 = 26225235;    float lTnJGczmWrgyXqpYhThd66421596 = -108481120;    float lTnJGczmWrgyXqpYhThd46657834 = -736452778;    float lTnJGczmWrgyXqpYhThd65593155 = -953433230;    float lTnJGczmWrgyXqpYhThd18555208 = -576110921;    float lTnJGczmWrgyXqpYhThd33935094 = -32860575;    float lTnJGczmWrgyXqpYhThd71006564 = -62677109;    float lTnJGczmWrgyXqpYhThd19365567 = -876915776;    float lTnJGczmWrgyXqpYhThd35164902 = -501756343;    float lTnJGczmWrgyXqpYhThd71978423 = -888700418;    float lTnJGczmWrgyXqpYhThd86071863 = 10127581;    float lTnJGczmWrgyXqpYhThd30167722 = -990708363;    float lTnJGczmWrgyXqpYhThd99050232 = -488404748;    float lTnJGczmWrgyXqpYhThd94243440 = -575043036;    float lTnJGczmWrgyXqpYhThd14314688 = -43214350;    float lTnJGczmWrgyXqpYhThd80309927 = 85009572;    float lTnJGczmWrgyXqpYhThd54519721 = -11678110;    float lTnJGczmWrgyXqpYhThd66331460 = -254855400;    float lTnJGczmWrgyXqpYhThd43512723 = -884158343;    float lTnJGczmWrgyXqpYhThd21789176 = -218431933;    float lTnJGczmWrgyXqpYhThd3114613 = -865242627;    float lTnJGczmWrgyXqpYhThd83416169 = -785137145;    float lTnJGczmWrgyXqpYhThd19507210 = -589930494;    float lTnJGczmWrgyXqpYhThd74371222 = -235404167;    float lTnJGczmWrgyXqpYhThd10131376 = 16272049;    float lTnJGczmWrgyXqpYhThd49907340 = -287919200;    float lTnJGczmWrgyXqpYhThd54757960 = -956138390;    float lTnJGczmWrgyXqpYhThd57643398 = -884776362;    float lTnJGczmWrgyXqpYhThd63269743 = -540856098;    float lTnJGczmWrgyXqpYhThd82039144 = -996146908;    float lTnJGczmWrgyXqpYhThd95108593 = -377045563;    float lTnJGczmWrgyXqpYhThd19024320 = -862308072;    float lTnJGczmWrgyXqpYhThd30158880 = -455477957;    float lTnJGczmWrgyXqpYhThd11857418 = -513445500;    float lTnJGczmWrgyXqpYhThd43984219 = -741102614;    float lTnJGczmWrgyXqpYhThd84420353 = -993177987;    float lTnJGczmWrgyXqpYhThd43930961 = -377608322;    float lTnJGczmWrgyXqpYhThd2997042 = -787893839;    float lTnJGczmWrgyXqpYhThd62252225 = -451979619;    float lTnJGczmWrgyXqpYhThd90474751 = -398597745;    float lTnJGczmWrgyXqpYhThd17401031 = -435819817;     lTnJGczmWrgyXqpYhThd2760759 = lTnJGczmWrgyXqpYhThd75662737;     lTnJGczmWrgyXqpYhThd75662737 = lTnJGczmWrgyXqpYhThd81668969;     lTnJGczmWrgyXqpYhThd81668969 = lTnJGczmWrgyXqpYhThd38827550;     lTnJGczmWrgyXqpYhThd38827550 = lTnJGczmWrgyXqpYhThd77243189;     lTnJGczmWrgyXqpYhThd77243189 = lTnJGczmWrgyXqpYhThd31344748;     lTnJGczmWrgyXqpYhThd31344748 = lTnJGczmWrgyXqpYhThd55694190;     lTnJGczmWrgyXqpYhThd55694190 = lTnJGczmWrgyXqpYhThd99194298;     lTnJGczmWrgyXqpYhThd99194298 = lTnJGczmWrgyXqpYhThd35752767;     lTnJGczmWrgyXqpYhThd35752767 = lTnJGczmWrgyXqpYhThd57154554;     lTnJGczmWrgyXqpYhThd57154554 = lTnJGczmWrgyXqpYhThd77092326;     lTnJGczmWrgyXqpYhThd77092326 = lTnJGczmWrgyXqpYhThd37859575;     lTnJGczmWrgyXqpYhThd37859575 = lTnJGczmWrgyXqpYhThd11168214;     lTnJGczmWrgyXqpYhThd11168214 = lTnJGczmWrgyXqpYhThd22708882;     lTnJGczmWrgyXqpYhThd22708882 = lTnJGczmWrgyXqpYhThd12836354;     lTnJGczmWrgyXqpYhThd12836354 = lTnJGczmWrgyXqpYhThd31798801;     lTnJGczmWrgyXqpYhThd31798801 = lTnJGczmWrgyXqpYhThd75965913;     lTnJGczmWrgyXqpYhThd75965913 = lTnJGczmWrgyXqpYhThd84877538;     lTnJGczmWrgyXqpYhThd84877538 = lTnJGczmWrgyXqpYhThd65156637;     lTnJGczmWrgyXqpYhThd65156637 = lTnJGczmWrgyXqpYhThd70898722;     lTnJGczmWrgyXqpYhThd70898722 = lTnJGczmWrgyXqpYhThd23109918;     lTnJGczmWrgyXqpYhThd23109918 = lTnJGczmWrgyXqpYhThd47138893;     lTnJGczmWrgyXqpYhThd47138893 = lTnJGczmWrgyXqpYhThd67922567;     lTnJGczmWrgyXqpYhThd67922567 = lTnJGczmWrgyXqpYhThd10196770;     lTnJGczmWrgyXqpYhThd10196770 = lTnJGczmWrgyXqpYhThd73769287;     lTnJGczmWrgyXqpYhThd73769287 = lTnJGczmWrgyXqpYhThd30593311;     lTnJGczmWrgyXqpYhThd30593311 = lTnJGczmWrgyXqpYhThd64923151;     lTnJGczmWrgyXqpYhThd64923151 = lTnJGczmWrgyXqpYhThd9036356;     lTnJGczmWrgyXqpYhThd9036356 = lTnJGczmWrgyXqpYhThd33601143;     lTnJGczmWrgyXqpYhThd33601143 = lTnJGczmWrgyXqpYhThd17197559;     lTnJGczmWrgyXqpYhThd17197559 = lTnJGczmWrgyXqpYhThd23219461;     lTnJGczmWrgyXqpYhThd23219461 = lTnJGczmWrgyXqpYhThd6085763;     lTnJGczmWrgyXqpYhThd6085763 = lTnJGczmWrgyXqpYhThd18494008;     lTnJGczmWrgyXqpYhThd18494008 = lTnJGczmWrgyXqpYhThd76003311;     lTnJGczmWrgyXqpYhThd76003311 = lTnJGczmWrgyXqpYhThd50730459;     lTnJGczmWrgyXqpYhThd50730459 = lTnJGczmWrgyXqpYhThd26764490;     lTnJGczmWrgyXqpYhThd26764490 = lTnJGczmWrgyXqpYhThd1631079;     lTnJGczmWrgyXqpYhThd1631079 = lTnJGczmWrgyXqpYhThd76915680;     lTnJGczmWrgyXqpYhThd76915680 = lTnJGczmWrgyXqpYhThd90634097;     lTnJGczmWrgyXqpYhThd90634097 = lTnJGczmWrgyXqpYhThd50841950;     lTnJGczmWrgyXqpYhThd50841950 = lTnJGczmWrgyXqpYhThd90588795;     lTnJGczmWrgyXqpYhThd90588795 = lTnJGczmWrgyXqpYhThd68590197;     lTnJGczmWrgyXqpYhThd68590197 = lTnJGczmWrgyXqpYhThd80807433;     lTnJGczmWrgyXqpYhThd80807433 = lTnJGczmWrgyXqpYhThd24409845;     lTnJGczmWrgyXqpYhThd24409845 = lTnJGczmWrgyXqpYhThd88407594;     lTnJGczmWrgyXqpYhThd88407594 = lTnJGczmWrgyXqpYhThd70654674;     lTnJGczmWrgyXqpYhThd70654674 = lTnJGczmWrgyXqpYhThd47177142;     lTnJGczmWrgyXqpYhThd47177142 = lTnJGczmWrgyXqpYhThd45415941;     lTnJGczmWrgyXqpYhThd45415941 = lTnJGczmWrgyXqpYhThd34665133;     lTnJGczmWrgyXqpYhThd34665133 = lTnJGczmWrgyXqpYhThd23469768;     lTnJGczmWrgyXqpYhThd23469768 = lTnJGczmWrgyXqpYhThd67290219;     lTnJGczmWrgyXqpYhThd67290219 = lTnJGczmWrgyXqpYhThd68461500;     lTnJGczmWrgyXqpYhThd68461500 = lTnJGczmWrgyXqpYhThd48442365;     lTnJGczmWrgyXqpYhThd48442365 = lTnJGczmWrgyXqpYhThd55224265;     lTnJGczmWrgyXqpYhThd55224265 = lTnJGczmWrgyXqpYhThd93964167;     lTnJGczmWrgyXqpYhThd93964167 = lTnJGczmWrgyXqpYhThd55621866;     lTnJGczmWrgyXqpYhThd55621866 = lTnJGczmWrgyXqpYhThd7740170;     lTnJGczmWrgyXqpYhThd7740170 = lTnJGczmWrgyXqpYhThd71472199;     lTnJGczmWrgyXqpYhThd71472199 = lTnJGczmWrgyXqpYhThd65058262;     lTnJGczmWrgyXqpYhThd65058262 = lTnJGczmWrgyXqpYhThd46649879;     lTnJGczmWrgyXqpYhThd46649879 = lTnJGczmWrgyXqpYhThd66421596;     lTnJGczmWrgyXqpYhThd66421596 = lTnJGczmWrgyXqpYhThd46657834;     lTnJGczmWrgyXqpYhThd46657834 = lTnJGczmWrgyXqpYhThd65593155;     lTnJGczmWrgyXqpYhThd65593155 = lTnJGczmWrgyXqpYhThd18555208;     lTnJGczmWrgyXqpYhThd18555208 = lTnJGczmWrgyXqpYhThd33935094;     lTnJGczmWrgyXqpYhThd33935094 = lTnJGczmWrgyXqpYhThd71006564;     lTnJGczmWrgyXqpYhThd71006564 = lTnJGczmWrgyXqpYhThd19365567;     lTnJGczmWrgyXqpYhThd19365567 = lTnJGczmWrgyXqpYhThd35164902;     lTnJGczmWrgyXqpYhThd35164902 = lTnJGczmWrgyXqpYhThd71978423;     lTnJGczmWrgyXqpYhThd71978423 = lTnJGczmWrgyXqpYhThd86071863;     lTnJGczmWrgyXqpYhThd86071863 = lTnJGczmWrgyXqpYhThd30167722;     lTnJGczmWrgyXqpYhThd30167722 = lTnJGczmWrgyXqpYhThd99050232;     lTnJGczmWrgyXqpYhThd99050232 = lTnJGczmWrgyXqpYhThd94243440;     lTnJGczmWrgyXqpYhThd94243440 = lTnJGczmWrgyXqpYhThd14314688;     lTnJGczmWrgyXqpYhThd14314688 = lTnJGczmWrgyXqpYhThd80309927;     lTnJGczmWrgyXqpYhThd80309927 = lTnJGczmWrgyXqpYhThd54519721;     lTnJGczmWrgyXqpYhThd54519721 = lTnJGczmWrgyXqpYhThd66331460;     lTnJGczmWrgyXqpYhThd66331460 = lTnJGczmWrgyXqpYhThd43512723;     lTnJGczmWrgyXqpYhThd43512723 = lTnJGczmWrgyXqpYhThd21789176;     lTnJGczmWrgyXqpYhThd21789176 = lTnJGczmWrgyXqpYhThd3114613;     lTnJGczmWrgyXqpYhThd3114613 = lTnJGczmWrgyXqpYhThd83416169;     lTnJGczmWrgyXqpYhThd83416169 = lTnJGczmWrgyXqpYhThd19507210;     lTnJGczmWrgyXqpYhThd19507210 = lTnJGczmWrgyXqpYhThd74371222;     lTnJGczmWrgyXqpYhThd74371222 = lTnJGczmWrgyXqpYhThd10131376;     lTnJGczmWrgyXqpYhThd10131376 = lTnJGczmWrgyXqpYhThd49907340;     lTnJGczmWrgyXqpYhThd49907340 = lTnJGczmWrgyXqpYhThd54757960;     lTnJGczmWrgyXqpYhThd54757960 = lTnJGczmWrgyXqpYhThd57643398;     lTnJGczmWrgyXqpYhThd57643398 = lTnJGczmWrgyXqpYhThd63269743;     lTnJGczmWrgyXqpYhThd63269743 = lTnJGczmWrgyXqpYhThd82039144;     lTnJGczmWrgyXqpYhThd82039144 = lTnJGczmWrgyXqpYhThd95108593;     lTnJGczmWrgyXqpYhThd95108593 = lTnJGczmWrgyXqpYhThd19024320;     lTnJGczmWrgyXqpYhThd19024320 = lTnJGczmWrgyXqpYhThd30158880;     lTnJGczmWrgyXqpYhThd30158880 = lTnJGczmWrgyXqpYhThd11857418;     lTnJGczmWrgyXqpYhThd11857418 = lTnJGczmWrgyXqpYhThd43984219;     lTnJGczmWrgyXqpYhThd43984219 = lTnJGczmWrgyXqpYhThd84420353;     lTnJGczmWrgyXqpYhThd84420353 = lTnJGczmWrgyXqpYhThd43930961;     lTnJGczmWrgyXqpYhThd43930961 = lTnJGczmWrgyXqpYhThd2997042;     lTnJGczmWrgyXqpYhThd2997042 = lTnJGczmWrgyXqpYhThd62252225;     lTnJGczmWrgyXqpYhThd62252225 = lTnJGczmWrgyXqpYhThd90474751;     lTnJGczmWrgyXqpYhThd90474751 = lTnJGczmWrgyXqpYhThd17401031;     lTnJGczmWrgyXqpYhThd17401031 = lTnJGczmWrgyXqpYhThd2760759;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void nfkarYOsjvXLUgTHplhG71490298() {     float pIRBdzaUPIKMEIxBZAfX73412426 = 46031053;    float pIRBdzaUPIKMEIxBZAfX44336255 = -739508665;    float pIRBdzaUPIKMEIxBZAfX61637888 = -634481763;    float pIRBdzaUPIKMEIxBZAfX78758836 = -655612427;    float pIRBdzaUPIKMEIxBZAfX98693511 = -857083843;    float pIRBdzaUPIKMEIxBZAfX21344510 = 49058693;    float pIRBdzaUPIKMEIxBZAfX74073085 = -182377052;    float pIRBdzaUPIKMEIxBZAfX56036679 = -971412641;    float pIRBdzaUPIKMEIxBZAfX81617820 = -269289617;    float pIRBdzaUPIKMEIxBZAfX80121015 = -867206446;    float pIRBdzaUPIKMEIxBZAfX19341488 = -52707463;    float pIRBdzaUPIKMEIxBZAfX83406145 = -609261880;    float pIRBdzaUPIKMEIxBZAfX90995600 = -363023799;    float pIRBdzaUPIKMEIxBZAfX6555916 = -119648069;    float pIRBdzaUPIKMEIxBZAfX95556372 = -454179198;    float pIRBdzaUPIKMEIxBZAfX64327441 = -53925335;    float pIRBdzaUPIKMEIxBZAfX69818740 = -114032396;    float pIRBdzaUPIKMEIxBZAfX5171033 = -379086611;    float pIRBdzaUPIKMEIxBZAfX46864419 = -952061397;    float pIRBdzaUPIKMEIxBZAfX29685432 = -671417832;    float pIRBdzaUPIKMEIxBZAfX78245672 = -761684319;    float pIRBdzaUPIKMEIxBZAfX8534658 = -714231245;    float pIRBdzaUPIKMEIxBZAfX69866376 = -983459806;    float pIRBdzaUPIKMEIxBZAfX44046307 = -846832781;    float pIRBdzaUPIKMEIxBZAfX37893676 = 61618293;    float pIRBdzaUPIKMEIxBZAfX61190891 = 1812824;    float pIRBdzaUPIKMEIxBZAfX73183936 = -436056812;    float pIRBdzaUPIKMEIxBZAfX16818581 = -952272332;    float pIRBdzaUPIKMEIxBZAfX65743631 = -965520538;    float pIRBdzaUPIKMEIxBZAfX56369654 = -58226439;    float pIRBdzaUPIKMEIxBZAfX30291457 = -181684207;    float pIRBdzaUPIKMEIxBZAfX50597224 = -183425588;    float pIRBdzaUPIKMEIxBZAfX1439369 = -363543920;    float pIRBdzaUPIKMEIxBZAfX41368286 = -515260082;    float pIRBdzaUPIKMEIxBZAfX28754696 = -276866221;    float pIRBdzaUPIKMEIxBZAfX86757748 = -293783324;    float pIRBdzaUPIKMEIxBZAfX45422565 = -681826594;    float pIRBdzaUPIKMEIxBZAfX29708169 = -500555626;    float pIRBdzaUPIKMEIxBZAfX69167386 = -770606016;    float pIRBdzaUPIKMEIxBZAfX45945248 = -719267469;    float pIRBdzaUPIKMEIxBZAfX50662079 = -498079978;    float pIRBdzaUPIKMEIxBZAfX97073188 = -510716138;    float pIRBdzaUPIKMEIxBZAfX29774393 = -14844591;    float pIRBdzaUPIKMEIxBZAfX30918146 = -603433290;    float pIRBdzaUPIKMEIxBZAfX84361652 = -144646337;    float pIRBdzaUPIKMEIxBZAfX77060509 = -155571548;    float pIRBdzaUPIKMEIxBZAfX73308345 = -230835385;    float pIRBdzaUPIKMEIxBZAfX11030151 = -903078839;    float pIRBdzaUPIKMEIxBZAfX40106586 = -97508367;    float pIRBdzaUPIKMEIxBZAfX31897255 = -870443249;    float pIRBdzaUPIKMEIxBZAfX35479543 = -858484290;    float pIRBdzaUPIKMEIxBZAfX10333005 = -446331854;    float pIRBdzaUPIKMEIxBZAfX3518022 = -998150106;    float pIRBdzaUPIKMEIxBZAfX45451569 = -436607710;    float pIRBdzaUPIKMEIxBZAfX93244625 = 33475595;    float pIRBdzaUPIKMEIxBZAfX64877768 = -239737703;    float pIRBdzaUPIKMEIxBZAfX74469878 = -756048859;    float pIRBdzaUPIKMEIxBZAfX17591582 = -787648983;    float pIRBdzaUPIKMEIxBZAfX40865160 = -617230720;    float pIRBdzaUPIKMEIxBZAfX37502620 = -758896668;    float pIRBdzaUPIKMEIxBZAfX48160574 = -514884496;    float pIRBdzaUPIKMEIxBZAfX57254505 = -230104720;    float pIRBdzaUPIKMEIxBZAfX90293048 = 94107897;    float pIRBdzaUPIKMEIxBZAfX25248166 = -111063179;    float pIRBdzaUPIKMEIxBZAfX49829558 = -585522240;    float pIRBdzaUPIKMEIxBZAfX68744264 = -869281876;    float pIRBdzaUPIKMEIxBZAfX81966777 = -145717960;    float pIRBdzaUPIKMEIxBZAfX49627314 = -847763717;    float pIRBdzaUPIKMEIxBZAfX77801220 = -842781849;    float pIRBdzaUPIKMEIxBZAfX8798625 = -60395875;    float pIRBdzaUPIKMEIxBZAfX18904877 = -372098742;    float pIRBdzaUPIKMEIxBZAfX40110571 = -613476770;    float pIRBdzaUPIKMEIxBZAfX36003646 = -608480596;    float pIRBdzaUPIKMEIxBZAfX919171 = -132793929;    float pIRBdzaUPIKMEIxBZAfX79023352 = -73337854;    float pIRBdzaUPIKMEIxBZAfX81172484 = -150968181;    float pIRBdzaUPIKMEIxBZAfX78760264 = -599386654;    float pIRBdzaUPIKMEIxBZAfX38948231 = -280026516;    float pIRBdzaUPIKMEIxBZAfX59684654 = -602186444;    float pIRBdzaUPIKMEIxBZAfX60833167 = -782810160;    float pIRBdzaUPIKMEIxBZAfX87882546 = -767351791;    float pIRBdzaUPIKMEIxBZAfX62153786 = -532977974;    float pIRBdzaUPIKMEIxBZAfX76711995 = -754763966;    float pIRBdzaUPIKMEIxBZAfX33846377 = 4922710;    float pIRBdzaUPIKMEIxBZAfX20890112 = -199742149;    float pIRBdzaUPIKMEIxBZAfX19958452 = -735352354;    float pIRBdzaUPIKMEIxBZAfX47079202 = -185275482;    float pIRBdzaUPIKMEIxBZAfX55987799 = -926936211;    float pIRBdzaUPIKMEIxBZAfX48123660 = -448735678;    float pIRBdzaUPIKMEIxBZAfX63876927 = 62871482;    float pIRBdzaUPIKMEIxBZAfX12287871 = -537734465;    float pIRBdzaUPIKMEIxBZAfX27830984 = -894177611;    float pIRBdzaUPIKMEIxBZAfX88843008 = -883324906;    float pIRBdzaUPIKMEIxBZAfX31664767 = 88290651;    float pIRBdzaUPIKMEIxBZAfX97784674 = -104382973;    float pIRBdzaUPIKMEIxBZAfX93407574 = -167975258;    float pIRBdzaUPIKMEIxBZAfX6780140 = -504824036;    float pIRBdzaUPIKMEIxBZAfX4526228 = -903781413;    float pIRBdzaUPIKMEIxBZAfX81088587 = 82088949;    float pIRBdzaUPIKMEIxBZAfX15617389 = 46031053;     pIRBdzaUPIKMEIxBZAfX73412426 = pIRBdzaUPIKMEIxBZAfX44336255;     pIRBdzaUPIKMEIxBZAfX44336255 = pIRBdzaUPIKMEIxBZAfX61637888;     pIRBdzaUPIKMEIxBZAfX61637888 = pIRBdzaUPIKMEIxBZAfX78758836;     pIRBdzaUPIKMEIxBZAfX78758836 = pIRBdzaUPIKMEIxBZAfX98693511;     pIRBdzaUPIKMEIxBZAfX98693511 = pIRBdzaUPIKMEIxBZAfX21344510;     pIRBdzaUPIKMEIxBZAfX21344510 = pIRBdzaUPIKMEIxBZAfX74073085;     pIRBdzaUPIKMEIxBZAfX74073085 = pIRBdzaUPIKMEIxBZAfX56036679;     pIRBdzaUPIKMEIxBZAfX56036679 = pIRBdzaUPIKMEIxBZAfX81617820;     pIRBdzaUPIKMEIxBZAfX81617820 = pIRBdzaUPIKMEIxBZAfX80121015;     pIRBdzaUPIKMEIxBZAfX80121015 = pIRBdzaUPIKMEIxBZAfX19341488;     pIRBdzaUPIKMEIxBZAfX19341488 = pIRBdzaUPIKMEIxBZAfX83406145;     pIRBdzaUPIKMEIxBZAfX83406145 = pIRBdzaUPIKMEIxBZAfX90995600;     pIRBdzaUPIKMEIxBZAfX90995600 = pIRBdzaUPIKMEIxBZAfX6555916;     pIRBdzaUPIKMEIxBZAfX6555916 = pIRBdzaUPIKMEIxBZAfX95556372;     pIRBdzaUPIKMEIxBZAfX95556372 = pIRBdzaUPIKMEIxBZAfX64327441;     pIRBdzaUPIKMEIxBZAfX64327441 = pIRBdzaUPIKMEIxBZAfX69818740;     pIRBdzaUPIKMEIxBZAfX69818740 = pIRBdzaUPIKMEIxBZAfX5171033;     pIRBdzaUPIKMEIxBZAfX5171033 = pIRBdzaUPIKMEIxBZAfX46864419;     pIRBdzaUPIKMEIxBZAfX46864419 = pIRBdzaUPIKMEIxBZAfX29685432;     pIRBdzaUPIKMEIxBZAfX29685432 = pIRBdzaUPIKMEIxBZAfX78245672;     pIRBdzaUPIKMEIxBZAfX78245672 = pIRBdzaUPIKMEIxBZAfX8534658;     pIRBdzaUPIKMEIxBZAfX8534658 = pIRBdzaUPIKMEIxBZAfX69866376;     pIRBdzaUPIKMEIxBZAfX69866376 = pIRBdzaUPIKMEIxBZAfX44046307;     pIRBdzaUPIKMEIxBZAfX44046307 = pIRBdzaUPIKMEIxBZAfX37893676;     pIRBdzaUPIKMEIxBZAfX37893676 = pIRBdzaUPIKMEIxBZAfX61190891;     pIRBdzaUPIKMEIxBZAfX61190891 = pIRBdzaUPIKMEIxBZAfX73183936;     pIRBdzaUPIKMEIxBZAfX73183936 = pIRBdzaUPIKMEIxBZAfX16818581;     pIRBdzaUPIKMEIxBZAfX16818581 = pIRBdzaUPIKMEIxBZAfX65743631;     pIRBdzaUPIKMEIxBZAfX65743631 = pIRBdzaUPIKMEIxBZAfX56369654;     pIRBdzaUPIKMEIxBZAfX56369654 = pIRBdzaUPIKMEIxBZAfX30291457;     pIRBdzaUPIKMEIxBZAfX30291457 = pIRBdzaUPIKMEIxBZAfX50597224;     pIRBdzaUPIKMEIxBZAfX50597224 = pIRBdzaUPIKMEIxBZAfX1439369;     pIRBdzaUPIKMEIxBZAfX1439369 = pIRBdzaUPIKMEIxBZAfX41368286;     pIRBdzaUPIKMEIxBZAfX41368286 = pIRBdzaUPIKMEIxBZAfX28754696;     pIRBdzaUPIKMEIxBZAfX28754696 = pIRBdzaUPIKMEIxBZAfX86757748;     pIRBdzaUPIKMEIxBZAfX86757748 = pIRBdzaUPIKMEIxBZAfX45422565;     pIRBdzaUPIKMEIxBZAfX45422565 = pIRBdzaUPIKMEIxBZAfX29708169;     pIRBdzaUPIKMEIxBZAfX29708169 = pIRBdzaUPIKMEIxBZAfX69167386;     pIRBdzaUPIKMEIxBZAfX69167386 = pIRBdzaUPIKMEIxBZAfX45945248;     pIRBdzaUPIKMEIxBZAfX45945248 = pIRBdzaUPIKMEIxBZAfX50662079;     pIRBdzaUPIKMEIxBZAfX50662079 = pIRBdzaUPIKMEIxBZAfX97073188;     pIRBdzaUPIKMEIxBZAfX97073188 = pIRBdzaUPIKMEIxBZAfX29774393;     pIRBdzaUPIKMEIxBZAfX29774393 = pIRBdzaUPIKMEIxBZAfX30918146;     pIRBdzaUPIKMEIxBZAfX30918146 = pIRBdzaUPIKMEIxBZAfX84361652;     pIRBdzaUPIKMEIxBZAfX84361652 = pIRBdzaUPIKMEIxBZAfX77060509;     pIRBdzaUPIKMEIxBZAfX77060509 = pIRBdzaUPIKMEIxBZAfX73308345;     pIRBdzaUPIKMEIxBZAfX73308345 = pIRBdzaUPIKMEIxBZAfX11030151;     pIRBdzaUPIKMEIxBZAfX11030151 = pIRBdzaUPIKMEIxBZAfX40106586;     pIRBdzaUPIKMEIxBZAfX40106586 = pIRBdzaUPIKMEIxBZAfX31897255;     pIRBdzaUPIKMEIxBZAfX31897255 = pIRBdzaUPIKMEIxBZAfX35479543;     pIRBdzaUPIKMEIxBZAfX35479543 = pIRBdzaUPIKMEIxBZAfX10333005;     pIRBdzaUPIKMEIxBZAfX10333005 = pIRBdzaUPIKMEIxBZAfX3518022;     pIRBdzaUPIKMEIxBZAfX3518022 = pIRBdzaUPIKMEIxBZAfX45451569;     pIRBdzaUPIKMEIxBZAfX45451569 = pIRBdzaUPIKMEIxBZAfX93244625;     pIRBdzaUPIKMEIxBZAfX93244625 = pIRBdzaUPIKMEIxBZAfX64877768;     pIRBdzaUPIKMEIxBZAfX64877768 = pIRBdzaUPIKMEIxBZAfX74469878;     pIRBdzaUPIKMEIxBZAfX74469878 = pIRBdzaUPIKMEIxBZAfX17591582;     pIRBdzaUPIKMEIxBZAfX17591582 = pIRBdzaUPIKMEIxBZAfX40865160;     pIRBdzaUPIKMEIxBZAfX40865160 = pIRBdzaUPIKMEIxBZAfX37502620;     pIRBdzaUPIKMEIxBZAfX37502620 = pIRBdzaUPIKMEIxBZAfX48160574;     pIRBdzaUPIKMEIxBZAfX48160574 = pIRBdzaUPIKMEIxBZAfX57254505;     pIRBdzaUPIKMEIxBZAfX57254505 = pIRBdzaUPIKMEIxBZAfX90293048;     pIRBdzaUPIKMEIxBZAfX90293048 = pIRBdzaUPIKMEIxBZAfX25248166;     pIRBdzaUPIKMEIxBZAfX25248166 = pIRBdzaUPIKMEIxBZAfX49829558;     pIRBdzaUPIKMEIxBZAfX49829558 = pIRBdzaUPIKMEIxBZAfX68744264;     pIRBdzaUPIKMEIxBZAfX68744264 = pIRBdzaUPIKMEIxBZAfX81966777;     pIRBdzaUPIKMEIxBZAfX81966777 = pIRBdzaUPIKMEIxBZAfX49627314;     pIRBdzaUPIKMEIxBZAfX49627314 = pIRBdzaUPIKMEIxBZAfX77801220;     pIRBdzaUPIKMEIxBZAfX77801220 = pIRBdzaUPIKMEIxBZAfX8798625;     pIRBdzaUPIKMEIxBZAfX8798625 = pIRBdzaUPIKMEIxBZAfX18904877;     pIRBdzaUPIKMEIxBZAfX18904877 = pIRBdzaUPIKMEIxBZAfX40110571;     pIRBdzaUPIKMEIxBZAfX40110571 = pIRBdzaUPIKMEIxBZAfX36003646;     pIRBdzaUPIKMEIxBZAfX36003646 = pIRBdzaUPIKMEIxBZAfX919171;     pIRBdzaUPIKMEIxBZAfX919171 = pIRBdzaUPIKMEIxBZAfX79023352;     pIRBdzaUPIKMEIxBZAfX79023352 = pIRBdzaUPIKMEIxBZAfX81172484;     pIRBdzaUPIKMEIxBZAfX81172484 = pIRBdzaUPIKMEIxBZAfX78760264;     pIRBdzaUPIKMEIxBZAfX78760264 = pIRBdzaUPIKMEIxBZAfX38948231;     pIRBdzaUPIKMEIxBZAfX38948231 = pIRBdzaUPIKMEIxBZAfX59684654;     pIRBdzaUPIKMEIxBZAfX59684654 = pIRBdzaUPIKMEIxBZAfX60833167;     pIRBdzaUPIKMEIxBZAfX60833167 = pIRBdzaUPIKMEIxBZAfX87882546;     pIRBdzaUPIKMEIxBZAfX87882546 = pIRBdzaUPIKMEIxBZAfX62153786;     pIRBdzaUPIKMEIxBZAfX62153786 = pIRBdzaUPIKMEIxBZAfX76711995;     pIRBdzaUPIKMEIxBZAfX76711995 = pIRBdzaUPIKMEIxBZAfX33846377;     pIRBdzaUPIKMEIxBZAfX33846377 = pIRBdzaUPIKMEIxBZAfX20890112;     pIRBdzaUPIKMEIxBZAfX20890112 = pIRBdzaUPIKMEIxBZAfX19958452;     pIRBdzaUPIKMEIxBZAfX19958452 = pIRBdzaUPIKMEIxBZAfX47079202;     pIRBdzaUPIKMEIxBZAfX47079202 = pIRBdzaUPIKMEIxBZAfX55987799;     pIRBdzaUPIKMEIxBZAfX55987799 = pIRBdzaUPIKMEIxBZAfX48123660;     pIRBdzaUPIKMEIxBZAfX48123660 = pIRBdzaUPIKMEIxBZAfX63876927;     pIRBdzaUPIKMEIxBZAfX63876927 = pIRBdzaUPIKMEIxBZAfX12287871;     pIRBdzaUPIKMEIxBZAfX12287871 = pIRBdzaUPIKMEIxBZAfX27830984;     pIRBdzaUPIKMEIxBZAfX27830984 = pIRBdzaUPIKMEIxBZAfX88843008;     pIRBdzaUPIKMEIxBZAfX88843008 = pIRBdzaUPIKMEIxBZAfX31664767;     pIRBdzaUPIKMEIxBZAfX31664767 = pIRBdzaUPIKMEIxBZAfX97784674;     pIRBdzaUPIKMEIxBZAfX97784674 = pIRBdzaUPIKMEIxBZAfX93407574;     pIRBdzaUPIKMEIxBZAfX93407574 = pIRBdzaUPIKMEIxBZAfX6780140;     pIRBdzaUPIKMEIxBZAfX6780140 = pIRBdzaUPIKMEIxBZAfX4526228;     pIRBdzaUPIKMEIxBZAfX4526228 = pIRBdzaUPIKMEIxBZAfX81088587;     pIRBdzaUPIKMEIxBZAfX81088587 = pIRBdzaUPIKMEIxBZAfX15617389;     pIRBdzaUPIKMEIxBZAfX15617389 = pIRBdzaUPIKMEIxBZAfX73412426;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void WjPZKfkjRwQNpodxERHS8465471() {     float oryYwWLlFekkYVmcopaz41138674 = -123369468;    float oryYwWLlFekkYVmcopaz20258452 = -151082031;    float oryYwWLlFekkYVmcopaz30987733 = -445879608;    float oryYwWLlFekkYVmcopaz7410444 = -476458378;    float oryYwWLlFekkYVmcopaz31256197 = -44060926;    float oryYwWLlFekkYVmcopaz56858662 = -481831661;    float oryYwWLlFekkYVmcopaz13675457 = -507008152;    float oryYwWLlFekkYVmcopaz71709485 = -645618370;    float oryYwWLlFekkYVmcopaz73746705 = -920278356;    float oryYwWLlFekkYVmcopaz85892128 = 32046619;    float oryYwWLlFekkYVmcopaz98592657 = -483447364;    float oryYwWLlFekkYVmcopaz90836058 = -409061447;    float oryYwWLlFekkYVmcopaz26039381 = -99859288;    float oryYwWLlFekkYVmcopaz10048458 = -280406299;    float oryYwWLlFekkYVmcopaz47239685 = -137248132;    float oryYwWLlFekkYVmcopaz93380365 = -38228964;    float oryYwWLlFekkYVmcopaz75663087 = -837326957;    float oryYwWLlFekkYVmcopaz66551394 = -64336456;    float oryYwWLlFekkYVmcopaz7353719 = -319334241;    float oryYwWLlFekkYVmcopaz62640922 = -90710590;    float oryYwWLlFekkYVmcopaz24623816 = -995554850;    float oryYwWLlFekkYVmcopaz99393553 = -462545333;    float oryYwWLlFekkYVmcopaz73568264 = 68245181;    float oryYwWLlFekkYVmcopaz12767834 = -492705344;    float oryYwWLlFekkYVmcopaz6123997 = -713721233;    float oryYwWLlFekkYVmcopaz24827016 = -346578543;    float oryYwWLlFekkYVmcopaz88505014 = 60401261;    float oryYwWLlFekkYVmcopaz4884556 = -241641913;    float oryYwWLlFekkYVmcopaz45416962 = -475603097;    float oryYwWLlFekkYVmcopaz47181295 = -463195711;    float oryYwWLlFekkYVmcopaz56289412 = -101221068;    float oryYwWLlFekkYVmcopaz61870661 = -815822176;    float oryYwWLlFekkYVmcopaz274586 = -538314017;    float oryYwWLlFekkYVmcopaz6383919 = 64052145;    float oryYwWLlFekkYVmcopaz52837941 = -382073308;    float oryYwWLlFekkYVmcopaz83952265 = -440948850;    float oryYwWLlFekkYVmcopaz76099157 = -664001303;    float oryYwWLlFekkYVmcopaz40274710 = -489514756;    float oryYwWLlFekkYVmcopaz2019466 = -402384862;    float oryYwWLlFekkYVmcopaz14017461 = -806614585;    float oryYwWLlFekkYVmcopaz11315717 = -976651384;    float oryYwWLlFekkYVmcopaz34604582 = -954884825;    float oryYwWLlFekkYVmcopaz4607763 = -621000111;    float oryYwWLlFekkYVmcopaz80599763 = -684081717;    float oryYwWLlFekkYVmcopaz76378908 = -294531618;    float oryYwWLlFekkYVmcopaz70255157 = -81055700;    float oryYwWLlFekkYVmcopaz96829823 = -163468877;    float oryYwWLlFekkYVmcopaz7585098 = -190722813;    float oryYwWLlFekkYVmcopaz10882114 = -811258738;    float oryYwWLlFekkYVmcopaz4184761 = -359469100;    float oryYwWLlFekkYVmcopaz82494590 = -34900073;    float oryYwWLlFekkYVmcopaz9349370 = -744230901;    float oryYwWLlFekkYVmcopaz25552770 = -822199887;    float oryYwWLlFekkYVmcopaz58572628 = -248321885;    float oryYwWLlFekkYVmcopaz83841654 = -568219116;    float oryYwWLlFekkYVmcopaz41745121 = -660824135;    float oryYwWLlFekkYVmcopaz46690188 = -119327212;    float oryYwWLlFekkYVmcopaz18219900 = -953174264;    float oryYwWLlFekkYVmcopaz1286448 = -762737145;    float oryYwWLlFekkYVmcopaz6429181 = -697482383;    float oryYwWLlFekkYVmcopaz68353648 = -442232923;    float oryYwWLlFekkYVmcopaz8790901 = -165366239;    float oryYwWLlFekkYVmcopaz26292524 = -70015273;    float oryYwWLlFekkYVmcopaz26565411 = -357082645;    float oryYwWLlFekkYVmcopaz29602716 = -866732313;    float oryYwWLlFekkYVmcopaz36721996 = -667625188;    float oryYwWLlFekkYVmcopaz90561472 = -870747430;    float oryYwWLlFekkYVmcopaz19655463 = -63911434;    float oryYwWLlFekkYVmcopaz57210516 = -898332991;    float oryYwWLlFekkYVmcopaz63287420 = -696299282;    float oryYwWLlFekkYVmcopaz17281209 = -374227661;    float oryYwWLlFekkYVmcopaz35388377 = -247812201;    float oryYwWLlFekkYVmcopaz64531929 = -661951594;    float oryYwWLlFekkYVmcopaz93336258 = -512719657;    float oryYwWLlFekkYVmcopaz51325206 = -114059207;    float oryYwWLlFekkYVmcopaz90019234 = 59329975;    float oryYwWLlFekkYVmcopaz94785790 = -841545222;    float oryYwWLlFekkYVmcopaz92968500 = -247673102;    float oryYwWLlFekkYVmcopaz36388926 = -98173726;    float oryYwWLlFekkYVmcopaz35868839 = -532665534;    float oryYwWLlFekkYVmcopaz27997193 = -83109667;    float oryYwWLlFekkYVmcopaz80919917 = -748875926;    float oryYwWLlFekkYVmcopaz94002442 = -430383176;    float oryYwWLlFekkYVmcopaz41232201 = -16133997;    float oryYwWLlFekkYVmcopaz64686705 = -328295639;    float oryYwWLlFekkYVmcopaz46940042 = -356990168;    float oryYwWLlFekkYVmcopaz36317892 = -993622289;    float oryYwWLlFekkYVmcopaz41701958 = -189992132;    float oryYwWLlFekkYVmcopaz22542264 = -367728739;    float oryYwWLlFekkYVmcopaz11092820 = -721249174;    float oryYwWLlFekkYVmcopaz37262078 = -221621638;    float oryYwWLlFekkYVmcopaz57879258 = -710827040;    float oryYwWLlFekkYVmcopaz38988263 = -726777611;    float oryYwWLlFekkYVmcopaz95590284 = -704902479;    float oryYwWLlFekkYVmcopaz45663812 = -264381662;    float oryYwWLlFekkYVmcopaz2524816 = -711285145;    float oryYwWLlFekkYVmcopaz8312059 = -784869552;    float oryYwWLlFekkYVmcopaz78042352 = -163917467;    float oryYwWLlFekkYVmcopaz50997048 = -817349404;    float oryYwWLlFekkYVmcopaz39656912 = -123369468;     oryYwWLlFekkYVmcopaz41138674 = oryYwWLlFekkYVmcopaz20258452;     oryYwWLlFekkYVmcopaz20258452 = oryYwWLlFekkYVmcopaz30987733;     oryYwWLlFekkYVmcopaz30987733 = oryYwWLlFekkYVmcopaz7410444;     oryYwWLlFekkYVmcopaz7410444 = oryYwWLlFekkYVmcopaz31256197;     oryYwWLlFekkYVmcopaz31256197 = oryYwWLlFekkYVmcopaz56858662;     oryYwWLlFekkYVmcopaz56858662 = oryYwWLlFekkYVmcopaz13675457;     oryYwWLlFekkYVmcopaz13675457 = oryYwWLlFekkYVmcopaz71709485;     oryYwWLlFekkYVmcopaz71709485 = oryYwWLlFekkYVmcopaz73746705;     oryYwWLlFekkYVmcopaz73746705 = oryYwWLlFekkYVmcopaz85892128;     oryYwWLlFekkYVmcopaz85892128 = oryYwWLlFekkYVmcopaz98592657;     oryYwWLlFekkYVmcopaz98592657 = oryYwWLlFekkYVmcopaz90836058;     oryYwWLlFekkYVmcopaz90836058 = oryYwWLlFekkYVmcopaz26039381;     oryYwWLlFekkYVmcopaz26039381 = oryYwWLlFekkYVmcopaz10048458;     oryYwWLlFekkYVmcopaz10048458 = oryYwWLlFekkYVmcopaz47239685;     oryYwWLlFekkYVmcopaz47239685 = oryYwWLlFekkYVmcopaz93380365;     oryYwWLlFekkYVmcopaz93380365 = oryYwWLlFekkYVmcopaz75663087;     oryYwWLlFekkYVmcopaz75663087 = oryYwWLlFekkYVmcopaz66551394;     oryYwWLlFekkYVmcopaz66551394 = oryYwWLlFekkYVmcopaz7353719;     oryYwWLlFekkYVmcopaz7353719 = oryYwWLlFekkYVmcopaz62640922;     oryYwWLlFekkYVmcopaz62640922 = oryYwWLlFekkYVmcopaz24623816;     oryYwWLlFekkYVmcopaz24623816 = oryYwWLlFekkYVmcopaz99393553;     oryYwWLlFekkYVmcopaz99393553 = oryYwWLlFekkYVmcopaz73568264;     oryYwWLlFekkYVmcopaz73568264 = oryYwWLlFekkYVmcopaz12767834;     oryYwWLlFekkYVmcopaz12767834 = oryYwWLlFekkYVmcopaz6123997;     oryYwWLlFekkYVmcopaz6123997 = oryYwWLlFekkYVmcopaz24827016;     oryYwWLlFekkYVmcopaz24827016 = oryYwWLlFekkYVmcopaz88505014;     oryYwWLlFekkYVmcopaz88505014 = oryYwWLlFekkYVmcopaz4884556;     oryYwWLlFekkYVmcopaz4884556 = oryYwWLlFekkYVmcopaz45416962;     oryYwWLlFekkYVmcopaz45416962 = oryYwWLlFekkYVmcopaz47181295;     oryYwWLlFekkYVmcopaz47181295 = oryYwWLlFekkYVmcopaz56289412;     oryYwWLlFekkYVmcopaz56289412 = oryYwWLlFekkYVmcopaz61870661;     oryYwWLlFekkYVmcopaz61870661 = oryYwWLlFekkYVmcopaz274586;     oryYwWLlFekkYVmcopaz274586 = oryYwWLlFekkYVmcopaz6383919;     oryYwWLlFekkYVmcopaz6383919 = oryYwWLlFekkYVmcopaz52837941;     oryYwWLlFekkYVmcopaz52837941 = oryYwWLlFekkYVmcopaz83952265;     oryYwWLlFekkYVmcopaz83952265 = oryYwWLlFekkYVmcopaz76099157;     oryYwWLlFekkYVmcopaz76099157 = oryYwWLlFekkYVmcopaz40274710;     oryYwWLlFekkYVmcopaz40274710 = oryYwWLlFekkYVmcopaz2019466;     oryYwWLlFekkYVmcopaz2019466 = oryYwWLlFekkYVmcopaz14017461;     oryYwWLlFekkYVmcopaz14017461 = oryYwWLlFekkYVmcopaz11315717;     oryYwWLlFekkYVmcopaz11315717 = oryYwWLlFekkYVmcopaz34604582;     oryYwWLlFekkYVmcopaz34604582 = oryYwWLlFekkYVmcopaz4607763;     oryYwWLlFekkYVmcopaz4607763 = oryYwWLlFekkYVmcopaz80599763;     oryYwWLlFekkYVmcopaz80599763 = oryYwWLlFekkYVmcopaz76378908;     oryYwWLlFekkYVmcopaz76378908 = oryYwWLlFekkYVmcopaz70255157;     oryYwWLlFekkYVmcopaz70255157 = oryYwWLlFekkYVmcopaz96829823;     oryYwWLlFekkYVmcopaz96829823 = oryYwWLlFekkYVmcopaz7585098;     oryYwWLlFekkYVmcopaz7585098 = oryYwWLlFekkYVmcopaz10882114;     oryYwWLlFekkYVmcopaz10882114 = oryYwWLlFekkYVmcopaz4184761;     oryYwWLlFekkYVmcopaz4184761 = oryYwWLlFekkYVmcopaz82494590;     oryYwWLlFekkYVmcopaz82494590 = oryYwWLlFekkYVmcopaz9349370;     oryYwWLlFekkYVmcopaz9349370 = oryYwWLlFekkYVmcopaz25552770;     oryYwWLlFekkYVmcopaz25552770 = oryYwWLlFekkYVmcopaz58572628;     oryYwWLlFekkYVmcopaz58572628 = oryYwWLlFekkYVmcopaz83841654;     oryYwWLlFekkYVmcopaz83841654 = oryYwWLlFekkYVmcopaz41745121;     oryYwWLlFekkYVmcopaz41745121 = oryYwWLlFekkYVmcopaz46690188;     oryYwWLlFekkYVmcopaz46690188 = oryYwWLlFekkYVmcopaz18219900;     oryYwWLlFekkYVmcopaz18219900 = oryYwWLlFekkYVmcopaz1286448;     oryYwWLlFekkYVmcopaz1286448 = oryYwWLlFekkYVmcopaz6429181;     oryYwWLlFekkYVmcopaz6429181 = oryYwWLlFekkYVmcopaz68353648;     oryYwWLlFekkYVmcopaz68353648 = oryYwWLlFekkYVmcopaz8790901;     oryYwWLlFekkYVmcopaz8790901 = oryYwWLlFekkYVmcopaz26292524;     oryYwWLlFekkYVmcopaz26292524 = oryYwWLlFekkYVmcopaz26565411;     oryYwWLlFekkYVmcopaz26565411 = oryYwWLlFekkYVmcopaz29602716;     oryYwWLlFekkYVmcopaz29602716 = oryYwWLlFekkYVmcopaz36721996;     oryYwWLlFekkYVmcopaz36721996 = oryYwWLlFekkYVmcopaz90561472;     oryYwWLlFekkYVmcopaz90561472 = oryYwWLlFekkYVmcopaz19655463;     oryYwWLlFekkYVmcopaz19655463 = oryYwWLlFekkYVmcopaz57210516;     oryYwWLlFekkYVmcopaz57210516 = oryYwWLlFekkYVmcopaz63287420;     oryYwWLlFekkYVmcopaz63287420 = oryYwWLlFekkYVmcopaz17281209;     oryYwWLlFekkYVmcopaz17281209 = oryYwWLlFekkYVmcopaz35388377;     oryYwWLlFekkYVmcopaz35388377 = oryYwWLlFekkYVmcopaz64531929;     oryYwWLlFekkYVmcopaz64531929 = oryYwWLlFekkYVmcopaz93336258;     oryYwWLlFekkYVmcopaz93336258 = oryYwWLlFekkYVmcopaz51325206;     oryYwWLlFekkYVmcopaz51325206 = oryYwWLlFekkYVmcopaz90019234;     oryYwWLlFekkYVmcopaz90019234 = oryYwWLlFekkYVmcopaz94785790;     oryYwWLlFekkYVmcopaz94785790 = oryYwWLlFekkYVmcopaz92968500;     oryYwWLlFekkYVmcopaz92968500 = oryYwWLlFekkYVmcopaz36388926;     oryYwWLlFekkYVmcopaz36388926 = oryYwWLlFekkYVmcopaz35868839;     oryYwWLlFekkYVmcopaz35868839 = oryYwWLlFekkYVmcopaz27997193;     oryYwWLlFekkYVmcopaz27997193 = oryYwWLlFekkYVmcopaz80919917;     oryYwWLlFekkYVmcopaz80919917 = oryYwWLlFekkYVmcopaz94002442;     oryYwWLlFekkYVmcopaz94002442 = oryYwWLlFekkYVmcopaz41232201;     oryYwWLlFekkYVmcopaz41232201 = oryYwWLlFekkYVmcopaz64686705;     oryYwWLlFekkYVmcopaz64686705 = oryYwWLlFekkYVmcopaz46940042;     oryYwWLlFekkYVmcopaz46940042 = oryYwWLlFekkYVmcopaz36317892;     oryYwWLlFekkYVmcopaz36317892 = oryYwWLlFekkYVmcopaz41701958;     oryYwWLlFekkYVmcopaz41701958 = oryYwWLlFekkYVmcopaz22542264;     oryYwWLlFekkYVmcopaz22542264 = oryYwWLlFekkYVmcopaz11092820;     oryYwWLlFekkYVmcopaz11092820 = oryYwWLlFekkYVmcopaz37262078;     oryYwWLlFekkYVmcopaz37262078 = oryYwWLlFekkYVmcopaz57879258;     oryYwWLlFekkYVmcopaz57879258 = oryYwWLlFekkYVmcopaz38988263;     oryYwWLlFekkYVmcopaz38988263 = oryYwWLlFekkYVmcopaz95590284;     oryYwWLlFekkYVmcopaz95590284 = oryYwWLlFekkYVmcopaz45663812;     oryYwWLlFekkYVmcopaz45663812 = oryYwWLlFekkYVmcopaz2524816;     oryYwWLlFekkYVmcopaz2524816 = oryYwWLlFekkYVmcopaz8312059;     oryYwWLlFekkYVmcopaz8312059 = oryYwWLlFekkYVmcopaz78042352;     oryYwWLlFekkYVmcopaz78042352 = oryYwWLlFekkYVmcopaz50997048;     oryYwWLlFekkYVmcopaz50997048 = oryYwWLlFekkYVmcopaz39656912;     oryYwWLlFekkYVmcopaz39656912 = oryYwWLlFekkYVmcopaz41138674;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void xGrbjihHsVFNKKVVgWBH62881683() {     float pLbqdZpqYRBBGnSGRIbx11790342 = -741518598;    float pLbqdZpqYRBBGnSGRIbx88931969 = -22725330;    float pLbqdZpqYRBBGnSGRIbx10956652 = -888174067;    float pLbqdZpqYRBBGnSGRIbx47341731 = -666596767;    float pLbqdZpqYRBBGnSGRIbx52706518 = -927467274;    float pLbqdZpqYRBBGnSGRIbx46858425 = -183412812;    float pLbqdZpqYRBBGnSGRIbx32054352 = -505783862;    float pLbqdZpqYRBBGnSGRIbx28551866 = -992435460;    float pLbqdZpqYRBBGnSGRIbx19611760 = 86374316;    float pLbqdZpqYRBBGnSGRIbx8858589 = -269048731;    float pLbqdZpqYRBBGnSGRIbx40841818 = -518349293;    float pLbqdZpqYRBBGnSGRIbx36382630 = -724825240;    float pLbqdZpqYRBBGnSGRIbx5866768 = -9396347;    float pLbqdZpqYRBBGnSGRIbx93895490 = -81434509;    float pLbqdZpqYRBBGnSGRIbx29959705 = -286998135;    float pLbqdZpqYRBBGnSGRIbx25909006 = -708212551;    float pLbqdZpqYRBBGnSGRIbx69515914 = -746512087;    float pLbqdZpqYRBBGnSGRIbx86844888 = -53502652;    float pLbqdZpqYRBBGnSGRIbx89061500 = 73477817;    float pLbqdZpqYRBBGnSGRIbx21427632 = -633076895;    float pLbqdZpqYRBBGnSGRIbx79759570 = -904233990;    float pLbqdZpqYRBBGnSGRIbx60789318 = -793830639;    float pLbqdZpqYRBBGnSGRIbx75512073 = -499597964;    float pLbqdZpqYRBBGnSGRIbx46617371 = -85106250;    float pLbqdZpqYRBBGnSGRIbx70248385 = -389625921;    float pLbqdZpqYRBBGnSGRIbx55424596 = -444862988;    float pLbqdZpqYRBBGnSGRIbx96765800 = -334776514;    float pLbqdZpqYRBBGnSGRIbx12666781 = -746765681;    float pLbqdZpqYRBBGnSGRIbx77559449 = -769961314;    float pLbqdZpqYRBBGnSGRIbx86353390 = 78409218;    float pLbqdZpqYRBBGnSGRIbx63361408 = -949654754;    float pLbqdZpqYRBBGnSGRIbx6382123 = -44119339;    float pLbqdZpqYRBBGnSGRIbx83219946 = -485275626;    float pLbqdZpqYRBBGnSGRIbx71748893 = -599477539;    float pLbqdZpqYRBBGnSGRIbx30862178 = -229020088;    float pLbqdZpqYRBBGnSGRIbx43945524 = -520175398;    float pLbqdZpqYRBBGnSGRIbx19890643 = -852594511;    float pLbqdZpqYRBBGnSGRIbx93067199 = -273627864;    float pLbqdZpqYRBBGnSGRIbx80552754 = -358113499;    float pLbqdZpqYRBBGnSGRIbx9120759 = -324222947;    float pLbqdZpqYRBBGnSGRIbx71389000 = -260670262;    float pLbqdZpqYRBBGnSGRIbx63087573 = -724273894;    float pLbqdZpqYRBBGnSGRIbx53574722 = -607754163;    float pLbqdZpqYRBBGnSGRIbx87108064 = -756056689;    float pLbqdZpqYRBBGnSGRIbx72332967 = -603178013;    float pLbqdZpqYRBBGnSGRIbx76660992 = -939392855;    float pLbqdZpqYRBBGnSGRIbx22961026 = -279538676;    float pLbqdZpqYRBBGnSGRIbx73199307 = -642853109;    float pLbqdZpqYRBBGnSGRIbx16323566 = -797022707;    float pLbqdZpqYRBBGnSGRIbx12612247 = -642477979;    float pLbqdZpqYRBBGnSGRIbx50683914 = -681472194;    float pLbqdZpqYRBBGnSGRIbx51220874 = -713450623;    float pLbqdZpqYRBBGnSGRIbx80628427 = -749997929;    float pLbqdZpqYRBBGnSGRIbx48799932 = -909203382;    float pLbqdZpqYRBBGnSGRIbx83122113 = -679160031;    float pLbqdZpqYRBBGnSGRIbx51001023 = -947687960;    float pLbqdZpqYRBBGnSGRIbx13419897 = -523127367;    float pLbqdZpqYRBBGnSGRIbx64339281 = -703067818;    float pLbqdZpqYRBBGnSGRIbx77093346 = -176970847;    float pLbqdZpqYRBBGnSGRIbx97281921 = -382604286;    float pLbqdZpqYRBBGnSGRIbx50092625 = -848636299;    float pLbqdZpqYRBBGnSGRIbx19387572 = -759018182;    float pLbqdZpqYRBBGnSGRIbx50992416 = -122474146;    float pLbqdZpqYRBBGnSGRIbx33258369 = -992034903;    float pLbqdZpqYRBBGnSGRIbx45497180 = -319393978;    float pLbqdZpqYRBBGnSGRIbx34459696 = -374229955;    float pLbqdZpqYRBBGnSGRIbx53162683 = -139549614;    float pLbqdZpqYRBBGnSGRIbx34117874 = -409918808;    float pLbqdZpqYRBBGnSGRIbx63033313 = -852414422;    float pLbqdZpqYRBBGnSGRIbx86014180 = -766822737;    float pLbqdZpqYRBBGnSGRIbx6018363 = -855618041;    float pLbqdZpqYRBBGnSGRIbx76448715 = -372884224;    float pLbqdZpqYRBBGnSGRIbx6292135 = -695389154;    float pLbqdZpqYRBBGnSGRIbx79940741 = -602299236;    float pLbqdZpqYRBBGnSGRIbx50038631 = -272406633;    float pLbqdZpqYRBBGnSGRIbx16671998 = -79960096;    float pLbqdZpqYRBBGnSGRIbx7214596 = -86076476;    float pLbqdZpqYRBBGnSGRIbx88404009 = -743541275;    float pLbqdZpqYRBBGnSGRIbx74284403 = -481928238;    float pLbqdZpqYRBBGnSGRIbx93587392 = -450233067;    float pLbqdZpqYRBBGnSGRIbx32463570 = -65324313;    float pLbqdZpqYRBBGnSGRIbx23566493 = -691923406;    float pLbqdZpqYRBBGnSGRIbx96343215 = -949742975;    float pLbqdZpqYRBBGnSGRIbx64947202 = -27483336;    float pLbqdZpqYRBBGnSGRIbx35669477 = -240118588;    float pLbqdZpqYRBBGnSGRIbx12140535 = -136204131;    float pLbqdZpqYRBBGnSGRIbx25753695 = -294121410;    float pLbqdZpqYRBBGnSGRIbx34420014 = -576072244;    float pLbqdZpqYRBBGnSGRIbx88626780 = -920317509;    float pLbqdZpqYRBBGnSGRIbx79861154 = -281332128;    float pLbqdZpqYRBBGnSGRIbx30525628 = -997048032;    float pLbqdZpqYRBBGnSGRIbx55551362 = -49526694;    float pLbqdZpqYRBBGnSGRIbx15973853 = 3342983;    float pLbqdZpqYRBBGnSGRIbx83270832 = -975509213;    float pLbqdZpqYRBBGnSGRIbx59028134 = -475586649;    float pLbqdZpqYRBBGnSGRIbx52001429 = -501652081;    float pLbqdZpqYRBBGnSGRIbx12095157 = -501799749;    float pLbqdZpqYRBBGnSGRIbx20316354 = -615719261;    float pLbqdZpqYRBBGnSGRIbx41610884 = -336662711;    float pLbqdZpqYRBBGnSGRIbx37873271 = -741518598;     pLbqdZpqYRBBGnSGRIbx11790342 = pLbqdZpqYRBBGnSGRIbx88931969;     pLbqdZpqYRBBGnSGRIbx88931969 = pLbqdZpqYRBBGnSGRIbx10956652;     pLbqdZpqYRBBGnSGRIbx10956652 = pLbqdZpqYRBBGnSGRIbx47341731;     pLbqdZpqYRBBGnSGRIbx47341731 = pLbqdZpqYRBBGnSGRIbx52706518;     pLbqdZpqYRBBGnSGRIbx52706518 = pLbqdZpqYRBBGnSGRIbx46858425;     pLbqdZpqYRBBGnSGRIbx46858425 = pLbqdZpqYRBBGnSGRIbx32054352;     pLbqdZpqYRBBGnSGRIbx32054352 = pLbqdZpqYRBBGnSGRIbx28551866;     pLbqdZpqYRBBGnSGRIbx28551866 = pLbqdZpqYRBBGnSGRIbx19611760;     pLbqdZpqYRBBGnSGRIbx19611760 = pLbqdZpqYRBBGnSGRIbx8858589;     pLbqdZpqYRBBGnSGRIbx8858589 = pLbqdZpqYRBBGnSGRIbx40841818;     pLbqdZpqYRBBGnSGRIbx40841818 = pLbqdZpqYRBBGnSGRIbx36382630;     pLbqdZpqYRBBGnSGRIbx36382630 = pLbqdZpqYRBBGnSGRIbx5866768;     pLbqdZpqYRBBGnSGRIbx5866768 = pLbqdZpqYRBBGnSGRIbx93895490;     pLbqdZpqYRBBGnSGRIbx93895490 = pLbqdZpqYRBBGnSGRIbx29959705;     pLbqdZpqYRBBGnSGRIbx29959705 = pLbqdZpqYRBBGnSGRIbx25909006;     pLbqdZpqYRBBGnSGRIbx25909006 = pLbqdZpqYRBBGnSGRIbx69515914;     pLbqdZpqYRBBGnSGRIbx69515914 = pLbqdZpqYRBBGnSGRIbx86844888;     pLbqdZpqYRBBGnSGRIbx86844888 = pLbqdZpqYRBBGnSGRIbx89061500;     pLbqdZpqYRBBGnSGRIbx89061500 = pLbqdZpqYRBBGnSGRIbx21427632;     pLbqdZpqYRBBGnSGRIbx21427632 = pLbqdZpqYRBBGnSGRIbx79759570;     pLbqdZpqYRBBGnSGRIbx79759570 = pLbqdZpqYRBBGnSGRIbx60789318;     pLbqdZpqYRBBGnSGRIbx60789318 = pLbqdZpqYRBBGnSGRIbx75512073;     pLbqdZpqYRBBGnSGRIbx75512073 = pLbqdZpqYRBBGnSGRIbx46617371;     pLbqdZpqYRBBGnSGRIbx46617371 = pLbqdZpqYRBBGnSGRIbx70248385;     pLbqdZpqYRBBGnSGRIbx70248385 = pLbqdZpqYRBBGnSGRIbx55424596;     pLbqdZpqYRBBGnSGRIbx55424596 = pLbqdZpqYRBBGnSGRIbx96765800;     pLbqdZpqYRBBGnSGRIbx96765800 = pLbqdZpqYRBBGnSGRIbx12666781;     pLbqdZpqYRBBGnSGRIbx12666781 = pLbqdZpqYRBBGnSGRIbx77559449;     pLbqdZpqYRBBGnSGRIbx77559449 = pLbqdZpqYRBBGnSGRIbx86353390;     pLbqdZpqYRBBGnSGRIbx86353390 = pLbqdZpqYRBBGnSGRIbx63361408;     pLbqdZpqYRBBGnSGRIbx63361408 = pLbqdZpqYRBBGnSGRIbx6382123;     pLbqdZpqYRBBGnSGRIbx6382123 = pLbqdZpqYRBBGnSGRIbx83219946;     pLbqdZpqYRBBGnSGRIbx83219946 = pLbqdZpqYRBBGnSGRIbx71748893;     pLbqdZpqYRBBGnSGRIbx71748893 = pLbqdZpqYRBBGnSGRIbx30862178;     pLbqdZpqYRBBGnSGRIbx30862178 = pLbqdZpqYRBBGnSGRIbx43945524;     pLbqdZpqYRBBGnSGRIbx43945524 = pLbqdZpqYRBBGnSGRIbx19890643;     pLbqdZpqYRBBGnSGRIbx19890643 = pLbqdZpqYRBBGnSGRIbx93067199;     pLbqdZpqYRBBGnSGRIbx93067199 = pLbqdZpqYRBBGnSGRIbx80552754;     pLbqdZpqYRBBGnSGRIbx80552754 = pLbqdZpqYRBBGnSGRIbx9120759;     pLbqdZpqYRBBGnSGRIbx9120759 = pLbqdZpqYRBBGnSGRIbx71389000;     pLbqdZpqYRBBGnSGRIbx71389000 = pLbqdZpqYRBBGnSGRIbx63087573;     pLbqdZpqYRBBGnSGRIbx63087573 = pLbqdZpqYRBBGnSGRIbx53574722;     pLbqdZpqYRBBGnSGRIbx53574722 = pLbqdZpqYRBBGnSGRIbx87108064;     pLbqdZpqYRBBGnSGRIbx87108064 = pLbqdZpqYRBBGnSGRIbx72332967;     pLbqdZpqYRBBGnSGRIbx72332967 = pLbqdZpqYRBBGnSGRIbx76660992;     pLbqdZpqYRBBGnSGRIbx76660992 = pLbqdZpqYRBBGnSGRIbx22961026;     pLbqdZpqYRBBGnSGRIbx22961026 = pLbqdZpqYRBBGnSGRIbx73199307;     pLbqdZpqYRBBGnSGRIbx73199307 = pLbqdZpqYRBBGnSGRIbx16323566;     pLbqdZpqYRBBGnSGRIbx16323566 = pLbqdZpqYRBBGnSGRIbx12612247;     pLbqdZpqYRBBGnSGRIbx12612247 = pLbqdZpqYRBBGnSGRIbx50683914;     pLbqdZpqYRBBGnSGRIbx50683914 = pLbqdZpqYRBBGnSGRIbx51220874;     pLbqdZpqYRBBGnSGRIbx51220874 = pLbqdZpqYRBBGnSGRIbx80628427;     pLbqdZpqYRBBGnSGRIbx80628427 = pLbqdZpqYRBBGnSGRIbx48799932;     pLbqdZpqYRBBGnSGRIbx48799932 = pLbqdZpqYRBBGnSGRIbx83122113;     pLbqdZpqYRBBGnSGRIbx83122113 = pLbqdZpqYRBBGnSGRIbx51001023;     pLbqdZpqYRBBGnSGRIbx51001023 = pLbqdZpqYRBBGnSGRIbx13419897;     pLbqdZpqYRBBGnSGRIbx13419897 = pLbqdZpqYRBBGnSGRIbx64339281;     pLbqdZpqYRBBGnSGRIbx64339281 = pLbqdZpqYRBBGnSGRIbx77093346;     pLbqdZpqYRBBGnSGRIbx77093346 = pLbqdZpqYRBBGnSGRIbx97281921;     pLbqdZpqYRBBGnSGRIbx97281921 = pLbqdZpqYRBBGnSGRIbx50092625;     pLbqdZpqYRBBGnSGRIbx50092625 = pLbqdZpqYRBBGnSGRIbx19387572;     pLbqdZpqYRBBGnSGRIbx19387572 = pLbqdZpqYRBBGnSGRIbx50992416;     pLbqdZpqYRBBGnSGRIbx50992416 = pLbqdZpqYRBBGnSGRIbx33258369;     pLbqdZpqYRBBGnSGRIbx33258369 = pLbqdZpqYRBBGnSGRIbx45497180;     pLbqdZpqYRBBGnSGRIbx45497180 = pLbqdZpqYRBBGnSGRIbx34459696;     pLbqdZpqYRBBGnSGRIbx34459696 = pLbqdZpqYRBBGnSGRIbx53162683;     pLbqdZpqYRBBGnSGRIbx53162683 = pLbqdZpqYRBBGnSGRIbx34117874;     pLbqdZpqYRBBGnSGRIbx34117874 = pLbqdZpqYRBBGnSGRIbx63033313;     pLbqdZpqYRBBGnSGRIbx63033313 = pLbqdZpqYRBBGnSGRIbx86014180;     pLbqdZpqYRBBGnSGRIbx86014180 = pLbqdZpqYRBBGnSGRIbx6018363;     pLbqdZpqYRBBGnSGRIbx6018363 = pLbqdZpqYRBBGnSGRIbx76448715;     pLbqdZpqYRBBGnSGRIbx76448715 = pLbqdZpqYRBBGnSGRIbx6292135;     pLbqdZpqYRBBGnSGRIbx6292135 = pLbqdZpqYRBBGnSGRIbx79940741;     pLbqdZpqYRBBGnSGRIbx79940741 = pLbqdZpqYRBBGnSGRIbx50038631;     pLbqdZpqYRBBGnSGRIbx50038631 = pLbqdZpqYRBBGnSGRIbx16671998;     pLbqdZpqYRBBGnSGRIbx16671998 = pLbqdZpqYRBBGnSGRIbx7214596;     pLbqdZpqYRBBGnSGRIbx7214596 = pLbqdZpqYRBBGnSGRIbx88404009;     pLbqdZpqYRBBGnSGRIbx88404009 = pLbqdZpqYRBBGnSGRIbx74284403;     pLbqdZpqYRBBGnSGRIbx74284403 = pLbqdZpqYRBBGnSGRIbx93587392;     pLbqdZpqYRBBGnSGRIbx93587392 = pLbqdZpqYRBBGnSGRIbx32463570;     pLbqdZpqYRBBGnSGRIbx32463570 = pLbqdZpqYRBBGnSGRIbx23566493;     pLbqdZpqYRBBGnSGRIbx23566493 = pLbqdZpqYRBBGnSGRIbx96343215;     pLbqdZpqYRBBGnSGRIbx96343215 = pLbqdZpqYRBBGnSGRIbx64947202;     pLbqdZpqYRBBGnSGRIbx64947202 = pLbqdZpqYRBBGnSGRIbx35669477;     pLbqdZpqYRBBGnSGRIbx35669477 = pLbqdZpqYRBBGnSGRIbx12140535;     pLbqdZpqYRBBGnSGRIbx12140535 = pLbqdZpqYRBBGnSGRIbx25753695;     pLbqdZpqYRBBGnSGRIbx25753695 = pLbqdZpqYRBBGnSGRIbx34420014;     pLbqdZpqYRBBGnSGRIbx34420014 = pLbqdZpqYRBBGnSGRIbx88626780;     pLbqdZpqYRBBGnSGRIbx88626780 = pLbqdZpqYRBBGnSGRIbx79861154;     pLbqdZpqYRBBGnSGRIbx79861154 = pLbqdZpqYRBBGnSGRIbx30525628;     pLbqdZpqYRBBGnSGRIbx30525628 = pLbqdZpqYRBBGnSGRIbx55551362;     pLbqdZpqYRBBGnSGRIbx55551362 = pLbqdZpqYRBBGnSGRIbx15973853;     pLbqdZpqYRBBGnSGRIbx15973853 = pLbqdZpqYRBBGnSGRIbx83270832;     pLbqdZpqYRBBGnSGRIbx83270832 = pLbqdZpqYRBBGnSGRIbx59028134;     pLbqdZpqYRBBGnSGRIbx59028134 = pLbqdZpqYRBBGnSGRIbx52001429;     pLbqdZpqYRBBGnSGRIbx52001429 = pLbqdZpqYRBBGnSGRIbx12095157;     pLbqdZpqYRBBGnSGRIbx12095157 = pLbqdZpqYRBBGnSGRIbx20316354;     pLbqdZpqYRBBGnSGRIbx20316354 = pLbqdZpqYRBBGnSGRIbx41610884;     pLbqdZpqYRBBGnSGRIbx41610884 = pLbqdZpqYRBBGnSGRIbx37873271;     pLbqdZpqYRBBGnSGRIbx37873271 = pLbqdZpqYRBBGnSGRIbx11790342;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void iQwKGgmwtKBlDGNaVOrj69540428() {     float gKwQykjaLAdFPBybXFQV11783870 = -614042740;    float gKwQykjaLAdFPBybXFQV27504286 = -65320455;    float gKwQykjaLAdFPBybXFQV12269864 = -845302681;    float gKwQykjaLAdFPBybXFQV32421592 = -487137597;    float gKwQykjaLAdFPBybXFQV61546620 = -295822595;    float gKwQykjaLAdFPBybXFQV81663857 = -982845624;    float gKwQykjaLAdFPBybXFQV601689 = -210320329;    float gKwQykjaLAdFPBybXFQV97765916 = -696612777;    float gKwQykjaLAdFPBybXFQV2351924 = -727271754;    float gKwQykjaLAdFPBybXFQV5498103 = 94144398;    float gKwQykjaLAdFPBybXFQV61162424 = -477821365;    float gKwQykjaLAdFPBybXFQV50674307 = -460303602;    float gKwQykjaLAdFPBybXFQV37719681 = -31054821;    float gKwQykjaLAdFPBybXFQV47739711 = -426587561;    float gKwQykjaLAdFPBybXFQV75131814 = -249710987;    float gKwQykjaLAdFPBybXFQV473553 = 58991798;    float gKwQykjaLAdFPBybXFQV25368674 = -566126657;    float gKwQykjaLAdFPBybXFQV9845422 = -450574274;    float gKwQykjaLAdFPBybXFQV9489770 = -544504450;    float gKwQykjaLAdFPBybXFQV93501394 = -145101346;    float gKwQykjaLAdFPBybXFQV59428995 = -614700364;    float gKwQykjaLAdFPBybXFQV39085584 = -203822522;    float gKwQykjaLAdFPBybXFQV12390470 = -958555806;    float gKwQykjaLAdFPBybXFQV90267479 = 33973227;    float gKwQykjaLAdFPBybXFQV98691074 = -816319774;    float gKwQykjaLAdFPBybXFQV71998673 = -353068917;    float gKwQykjaLAdFPBybXFQV66987382 = -635576227;    float gKwQykjaLAdFPBybXFQV48070306 = -255732669;    float gKwQykjaLAdFPBybXFQV90237896 = -316031629;    float gKwQykjaLAdFPBybXFQV62443261 = -697022155;    float gKwQykjaLAdFPBybXFQV96774087 = -236747988;    float gKwQykjaLAdFPBybXFQV91105979 = -435941100;    float gKwQykjaLAdFPBybXFQV68672369 = -687219842;    float gKwQykjaLAdFPBybXFQV72031730 = -476159272;    float gKwQykjaLAdFPBybXFQV13220216 = -488334013;    float gKwQykjaLAdFPBybXFQV86773713 = -599941144;    float gKwQykjaLAdFPBybXFQV31832011 = -952247889;    float gKwQykjaLAdFPBybXFQV62984878 = -268890543;    float gKwQykjaLAdFPBybXFQV15866351 = 29205085;    float gKwQykjaLAdFPBybXFQV80993651 = 66345366;    float gKwQykjaLAdFPBybXFQV59244668 = -348614161;    float gKwQykjaLAdFPBybXFQV48785234 = -918065977;    float gKwQykjaLAdFPBybXFQV13858083 = -66884418;    float gKwQykjaLAdFPBybXFQV46339962 = -313021132;    float gKwQykjaLAdFPBybXFQV56351019 = 23562586;    float gKwQykjaLAdFPBybXFQV17088961 = -751437525;    float gKwQykjaLAdFPBybXFQV50658819 = -760819298;    float gKwQykjaLAdFPBybXFQV48582888 = -151614465;    float gKwQykjaLAdFPBybXFQV34981955 = -360786568;    float gKwQykjaLAdFPBybXFQV32657670 = -229502865;    float gKwQykjaLAdFPBybXFQV63943284 = -260027201;    float gKwQykjaLAdFPBybXFQV65768132 = 4403629;    float gKwQykjaLAdFPBybXFQV56076775 = -642051937;    float gKwQykjaLAdFPBybXFQV59050203 = -341123233;    float gKwQykjaLAdFPBybXFQV10111434 = -436059309;    float gKwQykjaLAdFPBybXFQV72698286 = -310220218;    float gKwQykjaLAdFPBybXFQV15113817 = -106764650;    float gKwQykjaLAdFPBybXFQV22002385 = -779275909;    float gKwQykjaLAdFPBybXFQV33730517 = -670817824;    float gKwQykjaLAdFPBybXFQV89547946 = -942753679;    float gKwQykjaLAdFPBybXFQV14676476 = -247269398;    float gKwQykjaLAdFPBybXFQV52531382 = -954587660;    float gKwQykjaLAdFPBybXFQV7528020 = -280581149;    float gKwQykjaLAdFPBybXFQV39908663 = 69750401;    float gKwQykjaLAdFPBybXFQV8724015 = -669107615;    float gKwQykjaLAdFPBybXFQV70056444 = 58119735;    float gKwQykjaLAdFPBybXFQV82001937 = -773083761;    float gKwQykjaLAdFPBybXFQV65687951 = -554895550;    float gKwQykjaLAdFPBybXFQV34519496 = -938253549;    float gKwQykjaLAdFPBybXFQV88358100 = -649769843;    float gKwQykjaLAdFPBybXFQV68641542 = 11239687;    float gKwQykjaLAdFPBybXFQV62383795 = -197236114;    float gKwQykjaLAdFPBybXFQV93979070 = -379779359;    float gKwQykjaLAdFPBybXFQV28496119 = -510849817;    float gKwQykjaLAdFPBybXFQV34256726 = -796487186;    float gKwQykjaLAdFPBybXFQV10643761 = -696634388;    float gKwQykjaLAdFPBybXFQV25227502 = -36938105;    float gKwQykjaLAdFPBybXFQV66050507 = -545534674;    float gKwQykjaLAdFPBybXFQV33916460 = -989589359;    float gKwQykjaLAdFPBybXFQV81602113 = 35117751;    float gKwQykjaLAdFPBybXFQV21339855 = -592249619;    float gKwQykjaLAdFPBybXFQV18404494 = -383961762;    float gKwQykjaLAdFPBybXFQV13088351 = -894946102;    float gKwQykjaLAdFPBybXFQV57580226 = 13471236;    float gKwQykjaLAdFPBybXFQV98499976 = -336994955;    float gKwQykjaLAdFPBybXFQV31005956 = -141151618;    float gKwQykjaLAdFPBybXFQV35029204 = -793889163;    float gKwQykjaLAdFPBybXFQV9622167 = -246096609;    float gKwQykjaLAdFPBybXFQV61920297 = 59900036;    float gKwQykjaLAdFPBybXFQV40521929 = -78113795;    float gKwQykjaLAdFPBybXFQV71659897 = -393176495;    float gKwQykjaLAdFPBybXFQV9829626 = -72971981;    float gKwQykjaLAdFPBybXFQV29254362 = -598072719;    float gKwQykjaLAdFPBybXFQV26318404 = -28041236;    float gKwQykjaLAdFPBybXFQV66317176 = -686385236;    float gKwQykjaLAdFPBybXFQV6713286 = -394026501;    float gKwQykjaLAdFPBybXFQV41257214 = -537484829;    float gKwQykjaLAdFPBybXFQV73949419 = -36634819;    float gKwQykjaLAdFPBybXFQV37615948 = -643913518;    float gKwQykjaLAdFPBybXFQV86294575 = -614042740;     gKwQykjaLAdFPBybXFQV11783870 = gKwQykjaLAdFPBybXFQV27504286;     gKwQykjaLAdFPBybXFQV27504286 = gKwQykjaLAdFPBybXFQV12269864;     gKwQykjaLAdFPBybXFQV12269864 = gKwQykjaLAdFPBybXFQV32421592;     gKwQykjaLAdFPBybXFQV32421592 = gKwQykjaLAdFPBybXFQV61546620;     gKwQykjaLAdFPBybXFQV61546620 = gKwQykjaLAdFPBybXFQV81663857;     gKwQykjaLAdFPBybXFQV81663857 = gKwQykjaLAdFPBybXFQV601689;     gKwQykjaLAdFPBybXFQV601689 = gKwQykjaLAdFPBybXFQV97765916;     gKwQykjaLAdFPBybXFQV97765916 = gKwQykjaLAdFPBybXFQV2351924;     gKwQykjaLAdFPBybXFQV2351924 = gKwQykjaLAdFPBybXFQV5498103;     gKwQykjaLAdFPBybXFQV5498103 = gKwQykjaLAdFPBybXFQV61162424;     gKwQykjaLAdFPBybXFQV61162424 = gKwQykjaLAdFPBybXFQV50674307;     gKwQykjaLAdFPBybXFQV50674307 = gKwQykjaLAdFPBybXFQV37719681;     gKwQykjaLAdFPBybXFQV37719681 = gKwQykjaLAdFPBybXFQV47739711;     gKwQykjaLAdFPBybXFQV47739711 = gKwQykjaLAdFPBybXFQV75131814;     gKwQykjaLAdFPBybXFQV75131814 = gKwQykjaLAdFPBybXFQV473553;     gKwQykjaLAdFPBybXFQV473553 = gKwQykjaLAdFPBybXFQV25368674;     gKwQykjaLAdFPBybXFQV25368674 = gKwQykjaLAdFPBybXFQV9845422;     gKwQykjaLAdFPBybXFQV9845422 = gKwQykjaLAdFPBybXFQV9489770;     gKwQykjaLAdFPBybXFQV9489770 = gKwQykjaLAdFPBybXFQV93501394;     gKwQykjaLAdFPBybXFQV93501394 = gKwQykjaLAdFPBybXFQV59428995;     gKwQykjaLAdFPBybXFQV59428995 = gKwQykjaLAdFPBybXFQV39085584;     gKwQykjaLAdFPBybXFQV39085584 = gKwQykjaLAdFPBybXFQV12390470;     gKwQykjaLAdFPBybXFQV12390470 = gKwQykjaLAdFPBybXFQV90267479;     gKwQykjaLAdFPBybXFQV90267479 = gKwQykjaLAdFPBybXFQV98691074;     gKwQykjaLAdFPBybXFQV98691074 = gKwQykjaLAdFPBybXFQV71998673;     gKwQykjaLAdFPBybXFQV71998673 = gKwQykjaLAdFPBybXFQV66987382;     gKwQykjaLAdFPBybXFQV66987382 = gKwQykjaLAdFPBybXFQV48070306;     gKwQykjaLAdFPBybXFQV48070306 = gKwQykjaLAdFPBybXFQV90237896;     gKwQykjaLAdFPBybXFQV90237896 = gKwQykjaLAdFPBybXFQV62443261;     gKwQykjaLAdFPBybXFQV62443261 = gKwQykjaLAdFPBybXFQV96774087;     gKwQykjaLAdFPBybXFQV96774087 = gKwQykjaLAdFPBybXFQV91105979;     gKwQykjaLAdFPBybXFQV91105979 = gKwQykjaLAdFPBybXFQV68672369;     gKwQykjaLAdFPBybXFQV68672369 = gKwQykjaLAdFPBybXFQV72031730;     gKwQykjaLAdFPBybXFQV72031730 = gKwQykjaLAdFPBybXFQV13220216;     gKwQykjaLAdFPBybXFQV13220216 = gKwQykjaLAdFPBybXFQV86773713;     gKwQykjaLAdFPBybXFQV86773713 = gKwQykjaLAdFPBybXFQV31832011;     gKwQykjaLAdFPBybXFQV31832011 = gKwQykjaLAdFPBybXFQV62984878;     gKwQykjaLAdFPBybXFQV62984878 = gKwQykjaLAdFPBybXFQV15866351;     gKwQykjaLAdFPBybXFQV15866351 = gKwQykjaLAdFPBybXFQV80993651;     gKwQykjaLAdFPBybXFQV80993651 = gKwQykjaLAdFPBybXFQV59244668;     gKwQykjaLAdFPBybXFQV59244668 = gKwQykjaLAdFPBybXFQV48785234;     gKwQykjaLAdFPBybXFQV48785234 = gKwQykjaLAdFPBybXFQV13858083;     gKwQykjaLAdFPBybXFQV13858083 = gKwQykjaLAdFPBybXFQV46339962;     gKwQykjaLAdFPBybXFQV46339962 = gKwQykjaLAdFPBybXFQV56351019;     gKwQykjaLAdFPBybXFQV56351019 = gKwQykjaLAdFPBybXFQV17088961;     gKwQykjaLAdFPBybXFQV17088961 = gKwQykjaLAdFPBybXFQV50658819;     gKwQykjaLAdFPBybXFQV50658819 = gKwQykjaLAdFPBybXFQV48582888;     gKwQykjaLAdFPBybXFQV48582888 = gKwQykjaLAdFPBybXFQV34981955;     gKwQykjaLAdFPBybXFQV34981955 = gKwQykjaLAdFPBybXFQV32657670;     gKwQykjaLAdFPBybXFQV32657670 = gKwQykjaLAdFPBybXFQV63943284;     gKwQykjaLAdFPBybXFQV63943284 = gKwQykjaLAdFPBybXFQV65768132;     gKwQykjaLAdFPBybXFQV65768132 = gKwQykjaLAdFPBybXFQV56076775;     gKwQykjaLAdFPBybXFQV56076775 = gKwQykjaLAdFPBybXFQV59050203;     gKwQykjaLAdFPBybXFQV59050203 = gKwQykjaLAdFPBybXFQV10111434;     gKwQykjaLAdFPBybXFQV10111434 = gKwQykjaLAdFPBybXFQV72698286;     gKwQykjaLAdFPBybXFQV72698286 = gKwQykjaLAdFPBybXFQV15113817;     gKwQykjaLAdFPBybXFQV15113817 = gKwQykjaLAdFPBybXFQV22002385;     gKwQykjaLAdFPBybXFQV22002385 = gKwQykjaLAdFPBybXFQV33730517;     gKwQykjaLAdFPBybXFQV33730517 = gKwQykjaLAdFPBybXFQV89547946;     gKwQykjaLAdFPBybXFQV89547946 = gKwQykjaLAdFPBybXFQV14676476;     gKwQykjaLAdFPBybXFQV14676476 = gKwQykjaLAdFPBybXFQV52531382;     gKwQykjaLAdFPBybXFQV52531382 = gKwQykjaLAdFPBybXFQV7528020;     gKwQykjaLAdFPBybXFQV7528020 = gKwQykjaLAdFPBybXFQV39908663;     gKwQykjaLAdFPBybXFQV39908663 = gKwQykjaLAdFPBybXFQV8724015;     gKwQykjaLAdFPBybXFQV8724015 = gKwQykjaLAdFPBybXFQV70056444;     gKwQykjaLAdFPBybXFQV70056444 = gKwQykjaLAdFPBybXFQV82001937;     gKwQykjaLAdFPBybXFQV82001937 = gKwQykjaLAdFPBybXFQV65687951;     gKwQykjaLAdFPBybXFQV65687951 = gKwQykjaLAdFPBybXFQV34519496;     gKwQykjaLAdFPBybXFQV34519496 = gKwQykjaLAdFPBybXFQV88358100;     gKwQykjaLAdFPBybXFQV88358100 = gKwQykjaLAdFPBybXFQV68641542;     gKwQykjaLAdFPBybXFQV68641542 = gKwQykjaLAdFPBybXFQV62383795;     gKwQykjaLAdFPBybXFQV62383795 = gKwQykjaLAdFPBybXFQV93979070;     gKwQykjaLAdFPBybXFQV93979070 = gKwQykjaLAdFPBybXFQV28496119;     gKwQykjaLAdFPBybXFQV28496119 = gKwQykjaLAdFPBybXFQV34256726;     gKwQykjaLAdFPBybXFQV34256726 = gKwQykjaLAdFPBybXFQV10643761;     gKwQykjaLAdFPBybXFQV10643761 = gKwQykjaLAdFPBybXFQV25227502;     gKwQykjaLAdFPBybXFQV25227502 = gKwQykjaLAdFPBybXFQV66050507;     gKwQykjaLAdFPBybXFQV66050507 = gKwQykjaLAdFPBybXFQV33916460;     gKwQykjaLAdFPBybXFQV33916460 = gKwQykjaLAdFPBybXFQV81602113;     gKwQykjaLAdFPBybXFQV81602113 = gKwQykjaLAdFPBybXFQV21339855;     gKwQykjaLAdFPBybXFQV21339855 = gKwQykjaLAdFPBybXFQV18404494;     gKwQykjaLAdFPBybXFQV18404494 = gKwQykjaLAdFPBybXFQV13088351;     gKwQykjaLAdFPBybXFQV13088351 = gKwQykjaLAdFPBybXFQV57580226;     gKwQykjaLAdFPBybXFQV57580226 = gKwQykjaLAdFPBybXFQV98499976;     gKwQykjaLAdFPBybXFQV98499976 = gKwQykjaLAdFPBybXFQV31005956;     gKwQykjaLAdFPBybXFQV31005956 = gKwQykjaLAdFPBybXFQV35029204;     gKwQykjaLAdFPBybXFQV35029204 = gKwQykjaLAdFPBybXFQV9622167;     gKwQykjaLAdFPBybXFQV9622167 = gKwQykjaLAdFPBybXFQV61920297;     gKwQykjaLAdFPBybXFQV61920297 = gKwQykjaLAdFPBybXFQV40521929;     gKwQykjaLAdFPBybXFQV40521929 = gKwQykjaLAdFPBybXFQV71659897;     gKwQykjaLAdFPBybXFQV71659897 = gKwQykjaLAdFPBybXFQV9829626;     gKwQykjaLAdFPBybXFQV9829626 = gKwQykjaLAdFPBybXFQV29254362;     gKwQykjaLAdFPBybXFQV29254362 = gKwQykjaLAdFPBybXFQV26318404;     gKwQykjaLAdFPBybXFQV26318404 = gKwQykjaLAdFPBybXFQV66317176;     gKwQykjaLAdFPBybXFQV66317176 = gKwQykjaLAdFPBybXFQV6713286;     gKwQykjaLAdFPBybXFQV6713286 = gKwQykjaLAdFPBybXFQV41257214;     gKwQykjaLAdFPBybXFQV41257214 = gKwQykjaLAdFPBybXFQV73949419;     gKwQykjaLAdFPBybXFQV73949419 = gKwQykjaLAdFPBybXFQV37615948;     gKwQykjaLAdFPBybXFQV37615948 = gKwQykjaLAdFPBybXFQV86294575;     gKwQykjaLAdFPBybXFQV86294575 = gKwQykjaLAdFPBybXFQV11783870;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void kIBNXtjLWcZtnUNFBjir23956642() {     float gQsoPumpqjnmOmiLSGTD82435537 = -132191870;    float gQsoPumpqjnmOmiLSGTD96177803 = 63036246;    float gQsoPumpqjnmOmiLSGTD92238783 = -187597141;    float gQsoPumpqjnmOmiLSGTD72352878 = -677275987;    float gQsoPumpqjnmOmiLSGTD82996941 = -79228943;    float gQsoPumpqjnmOmiLSGTD71663620 = -684426776;    float gQsoPumpqjnmOmiLSGTD18980584 = -209096039;    float gQsoPumpqjnmOmiLSGTD54608297 = 56570133;    float gQsoPumpqjnmOmiLSGTD48216978 = -820619083;    float gQsoPumpqjnmOmiLSGTD28464563 = -206950953;    float gQsoPumpqjnmOmiLSGTD3411585 = -512723294;    float gQsoPumpqjnmOmiLSGTD96220877 = -776067395;    float gQsoPumpqjnmOmiLSGTD17547069 = 59408120;    float gQsoPumpqjnmOmiLSGTD31586745 = -227615771;    float gQsoPumpqjnmOmiLSGTD57851833 = -399460990;    float gQsoPumpqjnmOmiLSGTD33002193 = -610991788;    float gQsoPumpqjnmOmiLSGTD19221501 = -475311787;    float gQsoPumpqjnmOmiLSGTD30138916 = -439740470;    float gQsoPumpqjnmOmiLSGTD91197551 = -151692392;    float gQsoPumpqjnmOmiLSGTD52288103 = -687467651;    float gQsoPumpqjnmOmiLSGTD14564750 = -523379504;    float gQsoPumpqjnmOmiLSGTD481349 = -535107828;    float gQsoPumpqjnmOmiLSGTD14334279 = -426398950;    float gQsoPumpqjnmOmiLSGTD24117016 = -658427679;    float gQsoPumpqjnmOmiLSGTD62815463 = -492224462;    float gQsoPumpqjnmOmiLSGTD2596255 = -451353361;    float gQsoPumpqjnmOmiLSGTD75248167 = 69245998;    float gQsoPumpqjnmOmiLSGTD55852531 = -760856437;    float gQsoPumpqjnmOmiLSGTD22380385 = -610389846;    float gQsoPumpqjnmOmiLSGTD1615357 = -155417226;    float gQsoPumpqjnmOmiLSGTD3846084 = 14818326;    float gQsoPumpqjnmOmiLSGTD35617441 = -764238263;    float gQsoPumpqjnmOmiLSGTD51617730 = -634181450;    float gQsoPumpqjnmOmiLSGTD37396705 = -39688956;    float gQsoPumpqjnmOmiLSGTD91244451 = -335280792;    float gQsoPumpqjnmOmiLSGTD46766973 = -679167692;    float gQsoPumpqjnmOmiLSGTD75623496 = -40841097;    float gQsoPumpqjnmOmiLSGTD15777367 = -53003650;    float gQsoPumpqjnmOmiLSGTD94399639 = 73476448;    float gQsoPumpqjnmOmiLSGTD76096949 = -551262996;    float gQsoPumpqjnmOmiLSGTD19317952 = -732633039;    float gQsoPumpqjnmOmiLSGTD77268225 = -687455046;    float gQsoPumpqjnmOmiLSGTD62825042 = -53638470;    float gQsoPumpqjnmOmiLSGTD52848263 = -384996104;    float gQsoPumpqjnmOmiLSGTD52305078 = -285083809;    float gQsoPumpqjnmOmiLSGTD23494796 = -509774680;    float gQsoPumpqjnmOmiLSGTD76790022 = -876889098;    float gQsoPumpqjnmOmiLSGTD14197098 = -603744760;    float gQsoPumpqjnmOmiLSGTD40423407 = -346550537;    float gQsoPumpqjnmOmiLSGTD41085157 = -512511744;    float gQsoPumpqjnmOmiLSGTD32132609 = -906599322;    float gQsoPumpqjnmOmiLSGTD7639637 = 35183907;    float gQsoPumpqjnmOmiLSGTD11152433 = -569849979;    float gQsoPumpqjnmOmiLSGTD49277508 = 97995270;    float gQsoPumpqjnmOmiLSGTD9391892 = -547000223;    float gQsoPumpqjnmOmiLSGTD81954188 = -597084043;    float gQsoPumpqjnmOmiLSGTD81843525 = -510564804;    float gQsoPumpqjnmOmiLSGTD68121767 = -529169462;    float gQsoPumpqjnmOmiLSGTD9537416 = -85051525;    float gQsoPumpqjnmOmiLSGTD80400687 = -627875582;    float gQsoPumpqjnmOmiLSGTD96415452 = -653672774;    float gQsoPumpqjnmOmiLSGTD63128052 = -448239603;    float gQsoPumpqjnmOmiLSGTD32227913 = -333040021;    float gQsoPumpqjnmOmiLSGTD46601622 = -565201857;    float gQsoPumpqjnmOmiLSGTD24618480 = -121769280;    float gQsoPumpqjnmOmiLSGTD67794144 = -748485032;    float gQsoPumpqjnmOmiLSGTD44603147 = -41885945;    float gQsoPumpqjnmOmiLSGTD80150363 = -900902924;    float gQsoPumpqjnmOmiLSGTD40342293 = -892334979;    float gQsoPumpqjnmOmiLSGTD11084861 = -720293298;    float gQsoPumpqjnmOmiLSGTD57378697 = -470150692;    float gQsoPumpqjnmOmiLSGTD3444134 = -322308137;    float gQsoPumpqjnmOmiLSGTD35739276 = -413216919;    float gQsoPumpqjnmOmiLSGTD15100602 = -600429396;    float gQsoPumpqjnmOmiLSGTD32970152 = -954834612;    float gQsoPumpqjnmOmiLSGTD37296524 = -835924459;    float gQsoPumpqjnmOmiLSGTD37656306 = -381469359;    float gQsoPumpqjnmOmiLSGTD61486015 = 58597153;    float gQsoPumpqjnmOmiLSGTD71811938 = -273343870;    float gQsoPumpqjnmOmiLSGTD39320667 = -982449782;    float gQsoPumpqjnmOmiLSGTD25806232 = -574464264;    float gQsoPumpqjnmOmiLSGTD61051069 = -327009242;    float gQsoPumpqjnmOmiLSGTD15429124 = -314305900;    float gQsoPumpqjnmOmiLSGTD81295228 = 2121898;    float gQsoPumpqjnmOmiLSGTD69482748 = -248817904;    float gQsoPumpqjnmOmiLSGTD96206447 = 79634419;    float gQsoPumpqjnmOmiLSGTD24465008 = -94388284;    float gQsoPumpqjnmOmiLSGTD2340223 = -632176721;    float gQsoPumpqjnmOmiLSGTD28004813 = -492688734;    float gQsoPumpqjnmOmiLSGTD9290264 = -738196749;    float gQsoPumpqjnmOmiLSGTD64923448 = -68602888;    float gQsoPumpqjnmOmiLSGTD7501730 = -511671635;    float gQsoPumpqjnmOmiLSGTD6239952 = -967952125;    float gQsoPumpqjnmOmiLSGTD13998953 = -298647970;    float gQsoPumpqjnmOmiLSGTD79681497 = -897590223;    float gQsoPumpqjnmOmiLSGTD56189899 = -184393437;    float gQsoPumpqjnmOmiLSGTD45040313 = -254415025;    float gQsoPumpqjnmOmiLSGTD16223421 = -488436613;    float gQsoPumpqjnmOmiLSGTD28229784 = -163226824;    float gQsoPumpqjnmOmiLSGTD84510934 = -132191870;     gQsoPumpqjnmOmiLSGTD82435537 = gQsoPumpqjnmOmiLSGTD96177803;     gQsoPumpqjnmOmiLSGTD96177803 = gQsoPumpqjnmOmiLSGTD92238783;     gQsoPumpqjnmOmiLSGTD92238783 = gQsoPumpqjnmOmiLSGTD72352878;     gQsoPumpqjnmOmiLSGTD72352878 = gQsoPumpqjnmOmiLSGTD82996941;     gQsoPumpqjnmOmiLSGTD82996941 = gQsoPumpqjnmOmiLSGTD71663620;     gQsoPumpqjnmOmiLSGTD71663620 = gQsoPumpqjnmOmiLSGTD18980584;     gQsoPumpqjnmOmiLSGTD18980584 = gQsoPumpqjnmOmiLSGTD54608297;     gQsoPumpqjnmOmiLSGTD54608297 = gQsoPumpqjnmOmiLSGTD48216978;     gQsoPumpqjnmOmiLSGTD48216978 = gQsoPumpqjnmOmiLSGTD28464563;     gQsoPumpqjnmOmiLSGTD28464563 = gQsoPumpqjnmOmiLSGTD3411585;     gQsoPumpqjnmOmiLSGTD3411585 = gQsoPumpqjnmOmiLSGTD96220877;     gQsoPumpqjnmOmiLSGTD96220877 = gQsoPumpqjnmOmiLSGTD17547069;     gQsoPumpqjnmOmiLSGTD17547069 = gQsoPumpqjnmOmiLSGTD31586745;     gQsoPumpqjnmOmiLSGTD31586745 = gQsoPumpqjnmOmiLSGTD57851833;     gQsoPumpqjnmOmiLSGTD57851833 = gQsoPumpqjnmOmiLSGTD33002193;     gQsoPumpqjnmOmiLSGTD33002193 = gQsoPumpqjnmOmiLSGTD19221501;     gQsoPumpqjnmOmiLSGTD19221501 = gQsoPumpqjnmOmiLSGTD30138916;     gQsoPumpqjnmOmiLSGTD30138916 = gQsoPumpqjnmOmiLSGTD91197551;     gQsoPumpqjnmOmiLSGTD91197551 = gQsoPumpqjnmOmiLSGTD52288103;     gQsoPumpqjnmOmiLSGTD52288103 = gQsoPumpqjnmOmiLSGTD14564750;     gQsoPumpqjnmOmiLSGTD14564750 = gQsoPumpqjnmOmiLSGTD481349;     gQsoPumpqjnmOmiLSGTD481349 = gQsoPumpqjnmOmiLSGTD14334279;     gQsoPumpqjnmOmiLSGTD14334279 = gQsoPumpqjnmOmiLSGTD24117016;     gQsoPumpqjnmOmiLSGTD24117016 = gQsoPumpqjnmOmiLSGTD62815463;     gQsoPumpqjnmOmiLSGTD62815463 = gQsoPumpqjnmOmiLSGTD2596255;     gQsoPumpqjnmOmiLSGTD2596255 = gQsoPumpqjnmOmiLSGTD75248167;     gQsoPumpqjnmOmiLSGTD75248167 = gQsoPumpqjnmOmiLSGTD55852531;     gQsoPumpqjnmOmiLSGTD55852531 = gQsoPumpqjnmOmiLSGTD22380385;     gQsoPumpqjnmOmiLSGTD22380385 = gQsoPumpqjnmOmiLSGTD1615357;     gQsoPumpqjnmOmiLSGTD1615357 = gQsoPumpqjnmOmiLSGTD3846084;     gQsoPumpqjnmOmiLSGTD3846084 = gQsoPumpqjnmOmiLSGTD35617441;     gQsoPumpqjnmOmiLSGTD35617441 = gQsoPumpqjnmOmiLSGTD51617730;     gQsoPumpqjnmOmiLSGTD51617730 = gQsoPumpqjnmOmiLSGTD37396705;     gQsoPumpqjnmOmiLSGTD37396705 = gQsoPumpqjnmOmiLSGTD91244451;     gQsoPumpqjnmOmiLSGTD91244451 = gQsoPumpqjnmOmiLSGTD46766973;     gQsoPumpqjnmOmiLSGTD46766973 = gQsoPumpqjnmOmiLSGTD75623496;     gQsoPumpqjnmOmiLSGTD75623496 = gQsoPumpqjnmOmiLSGTD15777367;     gQsoPumpqjnmOmiLSGTD15777367 = gQsoPumpqjnmOmiLSGTD94399639;     gQsoPumpqjnmOmiLSGTD94399639 = gQsoPumpqjnmOmiLSGTD76096949;     gQsoPumpqjnmOmiLSGTD76096949 = gQsoPumpqjnmOmiLSGTD19317952;     gQsoPumpqjnmOmiLSGTD19317952 = gQsoPumpqjnmOmiLSGTD77268225;     gQsoPumpqjnmOmiLSGTD77268225 = gQsoPumpqjnmOmiLSGTD62825042;     gQsoPumpqjnmOmiLSGTD62825042 = gQsoPumpqjnmOmiLSGTD52848263;     gQsoPumpqjnmOmiLSGTD52848263 = gQsoPumpqjnmOmiLSGTD52305078;     gQsoPumpqjnmOmiLSGTD52305078 = gQsoPumpqjnmOmiLSGTD23494796;     gQsoPumpqjnmOmiLSGTD23494796 = gQsoPumpqjnmOmiLSGTD76790022;     gQsoPumpqjnmOmiLSGTD76790022 = gQsoPumpqjnmOmiLSGTD14197098;     gQsoPumpqjnmOmiLSGTD14197098 = gQsoPumpqjnmOmiLSGTD40423407;     gQsoPumpqjnmOmiLSGTD40423407 = gQsoPumpqjnmOmiLSGTD41085157;     gQsoPumpqjnmOmiLSGTD41085157 = gQsoPumpqjnmOmiLSGTD32132609;     gQsoPumpqjnmOmiLSGTD32132609 = gQsoPumpqjnmOmiLSGTD7639637;     gQsoPumpqjnmOmiLSGTD7639637 = gQsoPumpqjnmOmiLSGTD11152433;     gQsoPumpqjnmOmiLSGTD11152433 = gQsoPumpqjnmOmiLSGTD49277508;     gQsoPumpqjnmOmiLSGTD49277508 = gQsoPumpqjnmOmiLSGTD9391892;     gQsoPumpqjnmOmiLSGTD9391892 = gQsoPumpqjnmOmiLSGTD81954188;     gQsoPumpqjnmOmiLSGTD81954188 = gQsoPumpqjnmOmiLSGTD81843525;     gQsoPumpqjnmOmiLSGTD81843525 = gQsoPumpqjnmOmiLSGTD68121767;     gQsoPumpqjnmOmiLSGTD68121767 = gQsoPumpqjnmOmiLSGTD9537416;     gQsoPumpqjnmOmiLSGTD9537416 = gQsoPumpqjnmOmiLSGTD80400687;     gQsoPumpqjnmOmiLSGTD80400687 = gQsoPumpqjnmOmiLSGTD96415452;     gQsoPumpqjnmOmiLSGTD96415452 = gQsoPumpqjnmOmiLSGTD63128052;     gQsoPumpqjnmOmiLSGTD63128052 = gQsoPumpqjnmOmiLSGTD32227913;     gQsoPumpqjnmOmiLSGTD32227913 = gQsoPumpqjnmOmiLSGTD46601622;     gQsoPumpqjnmOmiLSGTD46601622 = gQsoPumpqjnmOmiLSGTD24618480;     gQsoPumpqjnmOmiLSGTD24618480 = gQsoPumpqjnmOmiLSGTD67794144;     gQsoPumpqjnmOmiLSGTD67794144 = gQsoPumpqjnmOmiLSGTD44603147;     gQsoPumpqjnmOmiLSGTD44603147 = gQsoPumpqjnmOmiLSGTD80150363;     gQsoPumpqjnmOmiLSGTD80150363 = gQsoPumpqjnmOmiLSGTD40342293;     gQsoPumpqjnmOmiLSGTD40342293 = gQsoPumpqjnmOmiLSGTD11084861;     gQsoPumpqjnmOmiLSGTD11084861 = gQsoPumpqjnmOmiLSGTD57378697;     gQsoPumpqjnmOmiLSGTD57378697 = gQsoPumpqjnmOmiLSGTD3444134;     gQsoPumpqjnmOmiLSGTD3444134 = gQsoPumpqjnmOmiLSGTD35739276;     gQsoPumpqjnmOmiLSGTD35739276 = gQsoPumpqjnmOmiLSGTD15100602;     gQsoPumpqjnmOmiLSGTD15100602 = gQsoPumpqjnmOmiLSGTD32970152;     gQsoPumpqjnmOmiLSGTD32970152 = gQsoPumpqjnmOmiLSGTD37296524;     gQsoPumpqjnmOmiLSGTD37296524 = gQsoPumpqjnmOmiLSGTD37656306;     gQsoPumpqjnmOmiLSGTD37656306 = gQsoPumpqjnmOmiLSGTD61486015;     gQsoPumpqjnmOmiLSGTD61486015 = gQsoPumpqjnmOmiLSGTD71811938;     gQsoPumpqjnmOmiLSGTD71811938 = gQsoPumpqjnmOmiLSGTD39320667;     gQsoPumpqjnmOmiLSGTD39320667 = gQsoPumpqjnmOmiLSGTD25806232;     gQsoPumpqjnmOmiLSGTD25806232 = gQsoPumpqjnmOmiLSGTD61051069;     gQsoPumpqjnmOmiLSGTD61051069 = gQsoPumpqjnmOmiLSGTD15429124;     gQsoPumpqjnmOmiLSGTD15429124 = gQsoPumpqjnmOmiLSGTD81295228;     gQsoPumpqjnmOmiLSGTD81295228 = gQsoPumpqjnmOmiLSGTD69482748;     gQsoPumpqjnmOmiLSGTD69482748 = gQsoPumpqjnmOmiLSGTD96206447;     gQsoPumpqjnmOmiLSGTD96206447 = gQsoPumpqjnmOmiLSGTD24465008;     gQsoPumpqjnmOmiLSGTD24465008 = gQsoPumpqjnmOmiLSGTD2340223;     gQsoPumpqjnmOmiLSGTD2340223 = gQsoPumpqjnmOmiLSGTD28004813;     gQsoPumpqjnmOmiLSGTD28004813 = gQsoPumpqjnmOmiLSGTD9290264;     gQsoPumpqjnmOmiLSGTD9290264 = gQsoPumpqjnmOmiLSGTD64923448;     gQsoPumpqjnmOmiLSGTD64923448 = gQsoPumpqjnmOmiLSGTD7501730;     gQsoPumpqjnmOmiLSGTD7501730 = gQsoPumpqjnmOmiLSGTD6239952;     gQsoPumpqjnmOmiLSGTD6239952 = gQsoPumpqjnmOmiLSGTD13998953;     gQsoPumpqjnmOmiLSGTD13998953 = gQsoPumpqjnmOmiLSGTD79681497;     gQsoPumpqjnmOmiLSGTD79681497 = gQsoPumpqjnmOmiLSGTD56189899;     gQsoPumpqjnmOmiLSGTD56189899 = gQsoPumpqjnmOmiLSGTD45040313;     gQsoPumpqjnmOmiLSGTD45040313 = gQsoPumpqjnmOmiLSGTD16223421;     gQsoPumpqjnmOmiLSGTD16223421 = gQsoPumpqjnmOmiLSGTD28229784;     gQsoPumpqjnmOmiLSGTD28229784 = gQsoPumpqjnmOmiLSGTD84510934;     gQsoPumpqjnmOmiLSGTD84510934 = gQsoPumpqjnmOmiLSGTD82435537;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void dksgOXmZWTFYldXjWdGR30615386() {     float upVdpmqdjJrFoNpwIRAe82429065 = -4716012;    float upVdpmqdjJrFoNpwIRAe34750120 = 20441120;    float upVdpmqdjJrFoNpwIRAe93551995 = -144725754;    float upVdpmqdjJrFoNpwIRAe57432739 = -497816817;    float upVdpmqdjJrFoNpwIRAe91837044 = -547584264;    float upVdpmqdjJrFoNpwIRAe6469053 = -383859587;    float upVdpmqdjJrFoNpwIRAe87527919 = 86367494;    float upVdpmqdjJrFoNpwIRAe23822348 = -747607184;    float upVdpmqdjJrFoNpwIRAe30957143 = -534265153;    float upVdpmqdjJrFoNpwIRAe25104077 = -943757824;    float upVdpmqdjJrFoNpwIRAe23732190 = -472195366;    float upVdpmqdjJrFoNpwIRAe10512555 = -511545758;    float upVdpmqdjJrFoNpwIRAe49399982 = 37749646;    float upVdpmqdjJrFoNpwIRAe85430964 = -572768822;    float upVdpmqdjJrFoNpwIRAe3023943 = -362173842;    float upVdpmqdjJrFoNpwIRAe7566741 = -943787439;    float upVdpmqdjJrFoNpwIRAe75074260 = -294926357;    float upVdpmqdjJrFoNpwIRAe53139449 = -836812092;    float upVdpmqdjJrFoNpwIRAe11625822 = -769674659;    float upVdpmqdjJrFoNpwIRAe24361867 = -199492102;    float upVdpmqdjJrFoNpwIRAe94234173 = -233845878;    float upVdpmqdjJrFoNpwIRAe78777614 = 54900289;    float upVdpmqdjJrFoNpwIRAe51212675 = -885356793;    float upVdpmqdjJrFoNpwIRAe67767124 = -539348201;    float upVdpmqdjJrFoNpwIRAe91258152 = -918918315;    float upVdpmqdjJrFoNpwIRAe19170331 = -359559290;    float upVdpmqdjJrFoNpwIRAe45469749 = -231553715;    float upVdpmqdjJrFoNpwIRAe91256056 = -269823425;    float upVdpmqdjJrFoNpwIRAe35058831 = -156460161;    float upVdpmqdjJrFoNpwIRAe77705226 = -930848600;    float upVdpmqdjJrFoNpwIRAe37258763 = -372274908;    float upVdpmqdjJrFoNpwIRAe20341298 = -56060025;    float upVdpmqdjJrFoNpwIRAe37070154 = -836125666;    float upVdpmqdjJrFoNpwIRAe37679543 = 83629311;    float upVdpmqdjJrFoNpwIRAe73602490 = -594594717;    float upVdpmqdjJrFoNpwIRAe89595162 = -758933438;    float upVdpmqdjJrFoNpwIRAe87564864 = -140494475;    float upVdpmqdjJrFoNpwIRAe85695046 = -48266329;    float upVdpmqdjJrFoNpwIRAe29713237 = -639204968;    float upVdpmqdjJrFoNpwIRAe47969842 = -160694682;    float upVdpmqdjJrFoNpwIRAe7173620 = -820576938;    float upVdpmqdjJrFoNpwIRAe62965886 = -881247128;    float upVdpmqdjJrFoNpwIRAe23108403 = -612768724;    float upVdpmqdjJrFoNpwIRAe12080162 = 58039453;    float upVdpmqdjJrFoNpwIRAe36323130 = -758343210;    float upVdpmqdjJrFoNpwIRAe63922764 = -321819351;    float upVdpmqdjJrFoNpwIRAe4487815 = -258169720;    float upVdpmqdjJrFoNpwIRAe89580679 = -112506116;    float upVdpmqdjJrFoNpwIRAe59081796 = 89685601;    float upVdpmqdjJrFoNpwIRAe61130579 = -99536630;    float upVdpmqdjJrFoNpwIRAe45391978 = -485154329;    float upVdpmqdjJrFoNpwIRAe22186894 = -346961841;    float upVdpmqdjJrFoNpwIRAe86600780 = -461903988;    float upVdpmqdjJrFoNpwIRAe59527778 = -433924581;    float upVdpmqdjJrFoNpwIRAe36381213 = -303899501;    float upVdpmqdjJrFoNpwIRAe3651452 = 40383699;    float upVdpmqdjJrFoNpwIRAe83537445 = -94202088;    float upVdpmqdjJrFoNpwIRAe25784871 = -605377554;    float upVdpmqdjJrFoNpwIRAe66174586 = -578898502;    float upVdpmqdjJrFoNpwIRAe72666713 = -88024975;    float upVdpmqdjJrFoNpwIRAe60999303 = -52305873;    float upVdpmqdjJrFoNpwIRAe96271863 = -643809081;    float upVdpmqdjJrFoNpwIRAe88763516 = -491147024;    float upVdpmqdjJrFoNpwIRAe53251916 = -603416553;    float upVdpmqdjJrFoNpwIRAe87845314 = -471482916;    float upVdpmqdjJrFoNpwIRAe3390892 = -316135342;    float upVdpmqdjJrFoNpwIRAe73442401 = -675420092;    float upVdpmqdjJrFoNpwIRAe11720440 = 54120334;    float upVdpmqdjJrFoNpwIRAe11828475 = -978174106;    float upVdpmqdjJrFoNpwIRAe13428780 = -603240404;    float upVdpmqdjJrFoNpwIRAe20001876 = -703292965;    float upVdpmqdjJrFoNpwIRAe89379213 = -146660028;    float upVdpmqdjJrFoNpwIRAe23426213 = -97607124;    float upVdpmqdjJrFoNpwIRAe63655979 = -508979977;    float upVdpmqdjJrFoNpwIRAe17188247 = -378915165;    float upVdpmqdjJrFoNpwIRAe31268288 = -352598750;    float upVdpmqdjJrFoNpwIRAe55669212 = -332330988;    float upVdpmqdjJrFoNpwIRAe39132514 = -843396246;    float upVdpmqdjJrFoNpwIRAe31443994 = -781004991;    float upVdpmqdjJrFoNpwIRAe27335389 = -497098964;    float upVdpmqdjJrFoNpwIRAe14682517 = -1389570;    float upVdpmqdjJrFoNpwIRAe55889070 = -19047599;    float upVdpmqdjJrFoNpwIRAe32174260 = -259509027;    float upVdpmqdjJrFoNpwIRAe73928252 = 43076469;    float upVdpmqdjJrFoNpwIRAe32313248 = -345694271;    float upVdpmqdjJrFoNpwIRAe15071869 = 74686933;    float upVdpmqdjJrFoNpwIRAe33740517 = -594156038;    float upVdpmqdjJrFoNpwIRAe77542375 = -302201086;    float upVdpmqdjJrFoNpwIRAe1298330 = -612471189;    float upVdpmqdjJrFoNpwIRAe69951038 = -534978416;    float upVdpmqdjJrFoNpwIRAe6057718 = -564731351;    float upVdpmqdjJrFoNpwIRAe61779993 = -535116922;    float upVdpmqdjJrFoNpwIRAe19520461 = -469367827;    float upVdpmqdjJrFoNpwIRAe57046524 = -451179993;    float upVdpmqdjJrFoNpwIRAe86970539 = -8388810;    float upVdpmqdjJrFoNpwIRAe10901756 = -76767857;    float upVdpmqdjJrFoNpwIRAe74202370 = -290100105;    float upVdpmqdjJrFoNpwIRAe69856487 = 90647829;    float upVdpmqdjJrFoNpwIRAe24234847 = -470477631;    float upVdpmqdjJrFoNpwIRAe32932239 = -4716012;     upVdpmqdjJrFoNpwIRAe82429065 = upVdpmqdjJrFoNpwIRAe34750120;     upVdpmqdjJrFoNpwIRAe34750120 = upVdpmqdjJrFoNpwIRAe93551995;     upVdpmqdjJrFoNpwIRAe93551995 = upVdpmqdjJrFoNpwIRAe57432739;     upVdpmqdjJrFoNpwIRAe57432739 = upVdpmqdjJrFoNpwIRAe91837044;     upVdpmqdjJrFoNpwIRAe91837044 = upVdpmqdjJrFoNpwIRAe6469053;     upVdpmqdjJrFoNpwIRAe6469053 = upVdpmqdjJrFoNpwIRAe87527919;     upVdpmqdjJrFoNpwIRAe87527919 = upVdpmqdjJrFoNpwIRAe23822348;     upVdpmqdjJrFoNpwIRAe23822348 = upVdpmqdjJrFoNpwIRAe30957143;     upVdpmqdjJrFoNpwIRAe30957143 = upVdpmqdjJrFoNpwIRAe25104077;     upVdpmqdjJrFoNpwIRAe25104077 = upVdpmqdjJrFoNpwIRAe23732190;     upVdpmqdjJrFoNpwIRAe23732190 = upVdpmqdjJrFoNpwIRAe10512555;     upVdpmqdjJrFoNpwIRAe10512555 = upVdpmqdjJrFoNpwIRAe49399982;     upVdpmqdjJrFoNpwIRAe49399982 = upVdpmqdjJrFoNpwIRAe85430964;     upVdpmqdjJrFoNpwIRAe85430964 = upVdpmqdjJrFoNpwIRAe3023943;     upVdpmqdjJrFoNpwIRAe3023943 = upVdpmqdjJrFoNpwIRAe7566741;     upVdpmqdjJrFoNpwIRAe7566741 = upVdpmqdjJrFoNpwIRAe75074260;     upVdpmqdjJrFoNpwIRAe75074260 = upVdpmqdjJrFoNpwIRAe53139449;     upVdpmqdjJrFoNpwIRAe53139449 = upVdpmqdjJrFoNpwIRAe11625822;     upVdpmqdjJrFoNpwIRAe11625822 = upVdpmqdjJrFoNpwIRAe24361867;     upVdpmqdjJrFoNpwIRAe24361867 = upVdpmqdjJrFoNpwIRAe94234173;     upVdpmqdjJrFoNpwIRAe94234173 = upVdpmqdjJrFoNpwIRAe78777614;     upVdpmqdjJrFoNpwIRAe78777614 = upVdpmqdjJrFoNpwIRAe51212675;     upVdpmqdjJrFoNpwIRAe51212675 = upVdpmqdjJrFoNpwIRAe67767124;     upVdpmqdjJrFoNpwIRAe67767124 = upVdpmqdjJrFoNpwIRAe91258152;     upVdpmqdjJrFoNpwIRAe91258152 = upVdpmqdjJrFoNpwIRAe19170331;     upVdpmqdjJrFoNpwIRAe19170331 = upVdpmqdjJrFoNpwIRAe45469749;     upVdpmqdjJrFoNpwIRAe45469749 = upVdpmqdjJrFoNpwIRAe91256056;     upVdpmqdjJrFoNpwIRAe91256056 = upVdpmqdjJrFoNpwIRAe35058831;     upVdpmqdjJrFoNpwIRAe35058831 = upVdpmqdjJrFoNpwIRAe77705226;     upVdpmqdjJrFoNpwIRAe77705226 = upVdpmqdjJrFoNpwIRAe37258763;     upVdpmqdjJrFoNpwIRAe37258763 = upVdpmqdjJrFoNpwIRAe20341298;     upVdpmqdjJrFoNpwIRAe20341298 = upVdpmqdjJrFoNpwIRAe37070154;     upVdpmqdjJrFoNpwIRAe37070154 = upVdpmqdjJrFoNpwIRAe37679543;     upVdpmqdjJrFoNpwIRAe37679543 = upVdpmqdjJrFoNpwIRAe73602490;     upVdpmqdjJrFoNpwIRAe73602490 = upVdpmqdjJrFoNpwIRAe89595162;     upVdpmqdjJrFoNpwIRAe89595162 = upVdpmqdjJrFoNpwIRAe87564864;     upVdpmqdjJrFoNpwIRAe87564864 = upVdpmqdjJrFoNpwIRAe85695046;     upVdpmqdjJrFoNpwIRAe85695046 = upVdpmqdjJrFoNpwIRAe29713237;     upVdpmqdjJrFoNpwIRAe29713237 = upVdpmqdjJrFoNpwIRAe47969842;     upVdpmqdjJrFoNpwIRAe47969842 = upVdpmqdjJrFoNpwIRAe7173620;     upVdpmqdjJrFoNpwIRAe7173620 = upVdpmqdjJrFoNpwIRAe62965886;     upVdpmqdjJrFoNpwIRAe62965886 = upVdpmqdjJrFoNpwIRAe23108403;     upVdpmqdjJrFoNpwIRAe23108403 = upVdpmqdjJrFoNpwIRAe12080162;     upVdpmqdjJrFoNpwIRAe12080162 = upVdpmqdjJrFoNpwIRAe36323130;     upVdpmqdjJrFoNpwIRAe36323130 = upVdpmqdjJrFoNpwIRAe63922764;     upVdpmqdjJrFoNpwIRAe63922764 = upVdpmqdjJrFoNpwIRAe4487815;     upVdpmqdjJrFoNpwIRAe4487815 = upVdpmqdjJrFoNpwIRAe89580679;     upVdpmqdjJrFoNpwIRAe89580679 = upVdpmqdjJrFoNpwIRAe59081796;     upVdpmqdjJrFoNpwIRAe59081796 = upVdpmqdjJrFoNpwIRAe61130579;     upVdpmqdjJrFoNpwIRAe61130579 = upVdpmqdjJrFoNpwIRAe45391978;     upVdpmqdjJrFoNpwIRAe45391978 = upVdpmqdjJrFoNpwIRAe22186894;     upVdpmqdjJrFoNpwIRAe22186894 = upVdpmqdjJrFoNpwIRAe86600780;     upVdpmqdjJrFoNpwIRAe86600780 = upVdpmqdjJrFoNpwIRAe59527778;     upVdpmqdjJrFoNpwIRAe59527778 = upVdpmqdjJrFoNpwIRAe36381213;     upVdpmqdjJrFoNpwIRAe36381213 = upVdpmqdjJrFoNpwIRAe3651452;     upVdpmqdjJrFoNpwIRAe3651452 = upVdpmqdjJrFoNpwIRAe83537445;     upVdpmqdjJrFoNpwIRAe83537445 = upVdpmqdjJrFoNpwIRAe25784871;     upVdpmqdjJrFoNpwIRAe25784871 = upVdpmqdjJrFoNpwIRAe66174586;     upVdpmqdjJrFoNpwIRAe66174586 = upVdpmqdjJrFoNpwIRAe72666713;     upVdpmqdjJrFoNpwIRAe72666713 = upVdpmqdjJrFoNpwIRAe60999303;     upVdpmqdjJrFoNpwIRAe60999303 = upVdpmqdjJrFoNpwIRAe96271863;     upVdpmqdjJrFoNpwIRAe96271863 = upVdpmqdjJrFoNpwIRAe88763516;     upVdpmqdjJrFoNpwIRAe88763516 = upVdpmqdjJrFoNpwIRAe53251916;     upVdpmqdjJrFoNpwIRAe53251916 = upVdpmqdjJrFoNpwIRAe87845314;     upVdpmqdjJrFoNpwIRAe87845314 = upVdpmqdjJrFoNpwIRAe3390892;     upVdpmqdjJrFoNpwIRAe3390892 = upVdpmqdjJrFoNpwIRAe73442401;     upVdpmqdjJrFoNpwIRAe73442401 = upVdpmqdjJrFoNpwIRAe11720440;     upVdpmqdjJrFoNpwIRAe11720440 = upVdpmqdjJrFoNpwIRAe11828475;     upVdpmqdjJrFoNpwIRAe11828475 = upVdpmqdjJrFoNpwIRAe13428780;     upVdpmqdjJrFoNpwIRAe13428780 = upVdpmqdjJrFoNpwIRAe20001876;     upVdpmqdjJrFoNpwIRAe20001876 = upVdpmqdjJrFoNpwIRAe89379213;     upVdpmqdjJrFoNpwIRAe89379213 = upVdpmqdjJrFoNpwIRAe23426213;     upVdpmqdjJrFoNpwIRAe23426213 = upVdpmqdjJrFoNpwIRAe63655979;     upVdpmqdjJrFoNpwIRAe63655979 = upVdpmqdjJrFoNpwIRAe17188247;     upVdpmqdjJrFoNpwIRAe17188247 = upVdpmqdjJrFoNpwIRAe31268288;     upVdpmqdjJrFoNpwIRAe31268288 = upVdpmqdjJrFoNpwIRAe55669212;     upVdpmqdjJrFoNpwIRAe55669212 = upVdpmqdjJrFoNpwIRAe39132514;     upVdpmqdjJrFoNpwIRAe39132514 = upVdpmqdjJrFoNpwIRAe31443994;     upVdpmqdjJrFoNpwIRAe31443994 = upVdpmqdjJrFoNpwIRAe27335389;     upVdpmqdjJrFoNpwIRAe27335389 = upVdpmqdjJrFoNpwIRAe14682517;     upVdpmqdjJrFoNpwIRAe14682517 = upVdpmqdjJrFoNpwIRAe55889070;     upVdpmqdjJrFoNpwIRAe55889070 = upVdpmqdjJrFoNpwIRAe32174260;     upVdpmqdjJrFoNpwIRAe32174260 = upVdpmqdjJrFoNpwIRAe73928252;     upVdpmqdjJrFoNpwIRAe73928252 = upVdpmqdjJrFoNpwIRAe32313248;     upVdpmqdjJrFoNpwIRAe32313248 = upVdpmqdjJrFoNpwIRAe15071869;     upVdpmqdjJrFoNpwIRAe15071869 = upVdpmqdjJrFoNpwIRAe33740517;     upVdpmqdjJrFoNpwIRAe33740517 = upVdpmqdjJrFoNpwIRAe77542375;     upVdpmqdjJrFoNpwIRAe77542375 = upVdpmqdjJrFoNpwIRAe1298330;     upVdpmqdjJrFoNpwIRAe1298330 = upVdpmqdjJrFoNpwIRAe69951038;     upVdpmqdjJrFoNpwIRAe69951038 = upVdpmqdjJrFoNpwIRAe6057718;     upVdpmqdjJrFoNpwIRAe6057718 = upVdpmqdjJrFoNpwIRAe61779993;     upVdpmqdjJrFoNpwIRAe61779993 = upVdpmqdjJrFoNpwIRAe19520461;     upVdpmqdjJrFoNpwIRAe19520461 = upVdpmqdjJrFoNpwIRAe57046524;     upVdpmqdjJrFoNpwIRAe57046524 = upVdpmqdjJrFoNpwIRAe86970539;     upVdpmqdjJrFoNpwIRAe86970539 = upVdpmqdjJrFoNpwIRAe10901756;     upVdpmqdjJrFoNpwIRAe10901756 = upVdpmqdjJrFoNpwIRAe74202370;     upVdpmqdjJrFoNpwIRAe74202370 = upVdpmqdjJrFoNpwIRAe69856487;     upVdpmqdjJrFoNpwIRAe69856487 = upVdpmqdjJrFoNpwIRAe24234847;     upVdpmqdjJrFoNpwIRAe24234847 = upVdpmqdjJrFoNpwIRAe32932239;     upVdpmqdjJrFoNpwIRAe32932239 = upVdpmqdjJrFoNpwIRAe82429065;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ENojpbHUfVkSQGruGVaN85031599() {     float rXzTelYTEYEOijRWdIxN53080733 = -622865142;    float rXzTelYTEYEOijRWdIxN3423638 = -951202179;    float rXzTelYTEYEOijRWdIxN73520914 = -587020214;    float rXzTelYTEYEOijRWdIxN97364025 = -687955206;    float rXzTelYTEYEOijRWdIxN13287366 = -330990611;    float rXzTelYTEYEOijRWdIxN96468814 = -85440739;    float rXzTelYTEYEOijRWdIxN5906816 = 87591784;    float rXzTelYTEYEOijRWdIxN80664728 = 5575726;    float rXzTelYTEYEOijRWdIxN76822196 = -627612481;    float rXzTelYTEYEOijRWdIxN48070537 = -144853175;    float rXzTelYTEYEOijRWdIxN65981350 = -507097296;    float rXzTelYTEYEOijRWdIxN56059126 = -827309551;    float rXzTelYTEYEOijRWdIxN29227369 = -971787412;    float rXzTelYTEYEOijRWdIxN69277998 = -373797032;    float rXzTelYTEYEOijRWdIxN85743961 = -511923845;    float rXzTelYTEYEOijRWdIxN40095381 = -513771026;    float rXzTelYTEYEOijRWdIxN68927087 = -204111486;    float rXzTelYTEYEOijRWdIxN73432943 = -825978288;    float rXzTelYTEYEOijRWdIxN93333603 = -376862601;    float rXzTelYTEYEOijRWdIxN83148575 = -741858407;    float rXzTelYTEYEOijRWdIxN49369928 = -142525019;    float rXzTelYTEYEOijRWdIxN40173379 = -276385016;    float rXzTelYTEYEOijRWdIxN53156484 = -353199937;    float rXzTelYTEYEOijRWdIxN1616662 = -131749108;    float rXzTelYTEYEOijRWdIxN55382541 = -594823003;    float rXzTelYTEYEOijRWdIxN49767912 = -457843735;    float rXzTelYTEYEOijRWdIxN53730535 = -626731490;    float rXzTelYTEYEOijRWdIxN99038281 = -774947193;    float rXzTelYTEYEOijRWdIxN67201319 = -450818378;    float rXzTelYTEYEOijRWdIxN16877323 = -389243670;    float rXzTelYTEYEOijRWdIxN44330759 = -120708594;    float rXzTelYTEYEOijRWdIxN64852758 = -384357188;    float rXzTelYTEYEOijRWdIxN20015515 = -783087275;    float rXzTelYTEYEOijRWdIxN3044518 = -579900373;    float rXzTelYTEYEOijRWdIxN51626726 = -441541496;    float rXzTelYTEYEOijRWdIxN49588421 = -838159986;    float rXzTelYTEYEOijRWdIxN31356350 = -329087682;    float rXzTelYTEYEOijRWdIxN38487535 = -932379437;    float rXzTelYTEYEOijRWdIxN8246526 = -594933604;    float rXzTelYTEYEOijRWdIxN43073141 = -778303045;    float rXzTelYTEYEOijRWdIxN67246903 = -104595816;    float rXzTelYTEYEOijRWdIxN91448877 = -650636198;    float rXzTelYTEYEOijRWdIxN72075362 = -599522776;    float rXzTelYTEYEOijRWdIxN18588462 = -13935519;    float rXzTelYTEYEOijRWdIxN32277189 = 33010395;    float rXzTelYTEYEOijRWdIxN70328599 = -80156506;    float rXzTelYTEYEOijRWdIxN30619018 = -374239519;    float rXzTelYTEYEOijRWdIxN55194889 = -564636411;    float rXzTelYTEYEOijRWdIxN64523249 = -996078368;    float rXzTelYTEYEOijRWdIxN69558066 = -382545509;    float rXzTelYTEYEOijRWdIxN13581303 = -31726450;    float rXzTelYTEYEOijRWdIxN64058398 = -316181563;    float rXzTelYTEYEOijRWdIxN41676438 = -389702030;    float rXzTelYTEYEOijRWdIxN49755083 = 5193922;    float rXzTelYTEYEOijRWdIxN35661671 = -414840415;    float rXzTelYTEYEOijRWdIxN12907354 = -246480126;    float rXzTelYTEYEOijRWdIxN50267154 = -498002242;    float rXzTelYTEYEOijRWdIxN71904253 = -355271107;    float rXzTelYTEYEOijRWdIxN41981485 = 6867796;    float rXzTelYTEYEOijRWdIxN63519454 = -873146877;    float rXzTelYTEYEOijRWdIxN42738280 = -458709249;    float rXzTelYTEYEOijRWdIxN6868534 = -137461024;    float rXzTelYTEYEOijRWdIxN13463409 = -543605897;    float rXzTelYTEYEOijRWdIxN59944874 = -138368811;    float rXzTelYTEYEOijRWdIxN3739779 = 75855419;    float rXzTelYTEYEOijRWdIxN1128592 = -22740109;    float rXzTelYTEYEOijRWdIxN36043612 = 55777724;    float rXzTelYTEYEOijRWdIxN26182852 = -291887040;    float rXzTelYTEYEOijRWdIxN17651272 = -932255537;    float rXzTelYTEYEOijRWdIxN36155540 = -673763859;    float rXzTelYTEYEOijRWdIxN8739031 = -84683344;    float rXzTelYTEYEOijRWdIxN30439552 = -271732050;    float rXzTelYTEYEOijRWdIxN65186418 = -131044684;    float rXzTelYTEYEOijRWdIxN50260463 = -598559556;    float rXzTelYTEYEOijRWdIxN15901673 = -537262592;    float rXzTelYTEYEOijRWdIxN57921051 = -491888821;    float rXzTelYTEYEOijRWdIxN68098016 = -676862241;    float rXzTelYTEYEOijRWdIxN34568022 = -239264419;    float rXzTelYTEYEOijRWdIxN69339472 = -64759503;    float rXzTelYTEYEOijRWdIxN85053942 = -414666497;    float rXzTelYTEYEOijRWdIxN19148894 = 16395784;    float rXzTelYTEYEOijRWdIxN98535645 = 37904921;    float rXzTelYTEYEOijRWdIxN34515033 = -778868825;    float rXzTelYTEYEOijRWdIxN97643253 = 31727131;    float rXzTelYTEYEOijRWdIxN3296020 = -257517220;    float rXzTelYTEYEOijRWdIxN80272361 = -804527031;    float rXzTelYTEYEOijRWdIxN23176321 = -994655158;    float rXzTelYTEYEOijRWdIxN70260431 = -688281198;    float rXzTelYTEYEOijRWdIxN67382846 = -65059959;    float rXzTelYTEYEOijRWdIxN38719373 = -95061371;    float rXzTelYTEYEOijRWdIxN99321267 = -240157745;    float rXzTelYTEYEOijRWdIxN59452097 = -973816576;    float rXzTelYTEYEOijRWdIxN96506050 = -839247234;    float rXzTelYTEYEOijRWdIxN44727072 = -721786728;    float rXzTelYTEYEOijRWdIxN334861 = -219593796;    float rXzTelYTEYEOijRWdIxN60378369 = -967134793;    float rXzTelYTEYEOijRWdIxN77985468 = -7030302;    float rXzTelYTEYEOijRWdIxN12130489 = -361153965;    float rXzTelYTEYEOijRWdIxN14848684 = 10209062;    float rXzTelYTEYEOijRWdIxN31148597 = -622865142;     rXzTelYTEYEOijRWdIxN53080733 = rXzTelYTEYEOijRWdIxN3423638;     rXzTelYTEYEOijRWdIxN3423638 = rXzTelYTEYEOijRWdIxN73520914;     rXzTelYTEYEOijRWdIxN73520914 = rXzTelYTEYEOijRWdIxN97364025;     rXzTelYTEYEOijRWdIxN97364025 = rXzTelYTEYEOijRWdIxN13287366;     rXzTelYTEYEOijRWdIxN13287366 = rXzTelYTEYEOijRWdIxN96468814;     rXzTelYTEYEOijRWdIxN96468814 = rXzTelYTEYEOijRWdIxN5906816;     rXzTelYTEYEOijRWdIxN5906816 = rXzTelYTEYEOijRWdIxN80664728;     rXzTelYTEYEOijRWdIxN80664728 = rXzTelYTEYEOijRWdIxN76822196;     rXzTelYTEYEOijRWdIxN76822196 = rXzTelYTEYEOijRWdIxN48070537;     rXzTelYTEYEOijRWdIxN48070537 = rXzTelYTEYEOijRWdIxN65981350;     rXzTelYTEYEOijRWdIxN65981350 = rXzTelYTEYEOijRWdIxN56059126;     rXzTelYTEYEOijRWdIxN56059126 = rXzTelYTEYEOijRWdIxN29227369;     rXzTelYTEYEOijRWdIxN29227369 = rXzTelYTEYEOijRWdIxN69277998;     rXzTelYTEYEOijRWdIxN69277998 = rXzTelYTEYEOijRWdIxN85743961;     rXzTelYTEYEOijRWdIxN85743961 = rXzTelYTEYEOijRWdIxN40095381;     rXzTelYTEYEOijRWdIxN40095381 = rXzTelYTEYEOijRWdIxN68927087;     rXzTelYTEYEOijRWdIxN68927087 = rXzTelYTEYEOijRWdIxN73432943;     rXzTelYTEYEOijRWdIxN73432943 = rXzTelYTEYEOijRWdIxN93333603;     rXzTelYTEYEOijRWdIxN93333603 = rXzTelYTEYEOijRWdIxN83148575;     rXzTelYTEYEOijRWdIxN83148575 = rXzTelYTEYEOijRWdIxN49369928;     rXzTelYTEYEOijRWdIxN49369928 = rXzTelYTEYEOijRWdIxN40173379;     rXzTelYTEYEOijRWdIxN40173379 = rXzTelYTEYEOijRWdIxN53156484;     rXzTelYTEYEOijRWdIxN53156484 = rXzTelYTEYEOijRWdIxN1616662;     rXzTelYTEYEOijRWdIxN1616662 = rXzTelYTEYEOijRWdIxN55382541;     rXzTelYTEYEOijRWdIxN55382541 = rXzTelYTEYEOijRWdIxN49767912;     rXzTelYTEYEOijRWdIxN49767912 = rXzTelYTEYEOijRWdIxN53730535;     rXzTelYTEYEOijRWdIxN53730535 = rXzTelYTEYEOijRWdIxN99038281;     rXzTelYTEYEOijRWdIxN99038281 = rXzTelYTEYEOijRWdIxN67201319;     rXzTelYTEYEOijRWdIxN67201319 = rXzTelYTEYEOijRWdIxN16877323;     rXzTelYTEYEOijRWdIxN16877323 = rXzTelYTEYEOijRWdIxN44330759;     rXzTelYTEYEOijRWdIxN44330759 = rXzTelYTEYEOijRWdIxN64852758;     rXzTelYTEYEOijRWdIxN64852758 = rXzTelYTEYEOijRWdIxN20015515;     rXzTelYTEYEOijRWdIxN20015515 = rXzTelYTEYEOijRWdIxN3044518;     rXzTelYTEYEOijRWdIxN3044518 = rXzTelYTEYEOijRWdIxN51626726;     rXzTelYTEYEOijRWdIxN51626726 = rXzTelYTEYEOijRWdIxN49588421;     rXzTelYTEYEOijRWdIxN49588421 = rXzTelYTEYEOijRWdIxN31356350;     rXzTelYTEYEOijRWdIxN31356350 = rXzTelYTEYEOijRWdIxN38487535;     rXzTelYTEYEOijRWdIxN38487535 = rXzTelYTEYEOijRWdIxN8246526;     rXzTelYTEYEOijRWdIxN8246526 = rXzTelYTEYEOijRWdIxN43073141;     rXzTelYTEYEOijRWdIxN43073141 = rXzTelYTEYEOijRWdIxN67246903;     rXzTelYTEYEOijRWdIxN67246903 = rXzTelYTEYEOijRWdIxN91448877;     rXzTelYTEYEOijRWdIxN91448877 = rXzTelYTEYEOijRWdIxN72075362;     rXzTelYTEYEOijRWdIxN72075362 = rXzTelYTEYEOijRWdIxN18588462;     rXzTelYTEYEOijRWdIxN18588462 = rXzTelYTEYEOijRWdIxN32277189;     rXzTelYTEYEOijRWdIxN32277189 = rXzTelYTEYEOijRWdIxN70328599;     rXzTelYTEYEOijRWdIxN70328599 = rXzTelYTEYEOijRWdIxN30619018;     rXzTelYTEYEOijRWdIxN30619018 = rXzTelYTEYEOijRWdIxN55194889;     rXzTelYTEYEOijRWdIxN55194889 = rXzTelYTEYEOijRWdIxN64523249;     rXzTelYTEYEOijRWdIxN64523249 = rXzTelYTEYEOijRWdIxN69558066;     rXzTelYTEYEOijRWdIxN69558066 = rXzTelYTEYEOijRWdIxN13581303;     rXzTelYTEYEOijRWdIxN13581303 = rXzTelYTEYEOijRWdIxN64058398;     rXzTelYTEYEOijRWdIxN64058398 = rXzTelYTEYEOijRWdIxN41676438;     rXzTelYTEYEOijRWdIxN41676438 = rXzTelYTEYEOijRWdIxN49755083;     rXzTelYTEYEOijRWdIxN49755083 = rXzTelYTEYEOijRWdIxN35661671;     rXzTelYTEYEOijRWdIxN35661671 = rXzTelYTEYEOijRWdIxN12907354;     rXzTelYTEYEOijRWdIxN12907354 = rXzTelYTEYEOijRWdIxN50267154;     rXzTelYTEYEOijRWdIxN50267154 = rXzTelYTEYEOijRWdIxN71904253;     rXzTelYTEYEOijRWdIxN71904253 = rXzTelYTEYEOijRWdIxN41981485;     rXzTelYTEYEOijRWdIxN41981485 = rXzTelYTEYEOijRWdIxN63519454;     rXzTelYTEYEOijRWdIxN63519454 = rXzTelYTEYEOijRWdIxN42738280;     rXzTelYTEYEOijRWdIxN42738280 = rXzTelYTEYEOijRWdIxN6868534;     rXzTelYTEYEOijRWdIxN6868534 = rXzTelYTEYEOijRWdIxN13463409;     rXzTelYTEYEOijRWdIxN13463409 = rXzTelYTEYEOijRWdIxN59944874;     rXzTelYTEYEOijRWdIxN59944874 = rXzTelYTEYEOijRWdIxN3739779;     rXzTelYTEYEOijRWdIxN3739779 = rXzTelYTEYEOijRWdIxN1128592;     rXzTelYTEYEOijRWdIxN1128592 = rXzTelYTEYEOijRWdIxN36043612;     rXzTelYTEYEOijRWdIxN36043612 = rXzTelYTEYEOijRWdIxN26182852;     rXzTelYTEYEOijRWdIxN26182852 = rXzTelYTEYEOijRWdIxN17651272;     rXzTelYTEYEOijRWdIxN17651272 = rXzTelYTEYEOijRWdIxN36155540;     rXzTelYTEYEOijRWdIxN36155540 = rXzTelYTEYEOijRWdIxN8739031;     rXzTelYTEYEOijRWdIxN8739031 = rXzTelYTEYEOijRWdIxN30439552;     rXzTelYTEYEOijRWdIxN30439552 = rXzTelYTEYEOijRWdIxN65186418;     rXzTelYTEYEOijRWdIxN65186418 = rXzTelYTEYEOijRWdIxN50260463;     rXzTelYTEYEOijRWdIxN50260463 = rXzTelYTEYEOijRWdIxN15901673;     rXzTelYTEYEOijRWdIxN15901673 = rXzTelYTEYEOijRWdIxN57921051;     rXzTelYTEYEOijRWdIxN57921051 = rXzTelYTEYEOijRWdIxN68098016;     rXzTelYTEYEOijRWdIxN68098016 = rXzTelYTEYEOijRWdIxN34568022;     rXzTelYTEYEOijRWdIxN34568022 = rXzTelYTEYEOijRWdIxN69339472;     rXzTelYTEYEOijRWdIxN69339472 = rXzTelYTEYEOijRWdIxN85053942;     rXzTelYTEYEOijRWdIxN85053942 = rXzTelYTEYEOijRWdIxN19148894;     rXzTelYTEYEOijRWdIxN19148894 = rXzTelYTEYEOijRWdIxN98535645;     rXzTelYTEYEOijRWdIxN98535645 = rXzTelYTEYEOijRWdIxN34515033;     rXzTelYTEYEOijRWdIxN34515033 = rXzTelYTEYEOijRWdIxN97643253;     rXzTelYTEYEOijRWdIxN97643253 = rXzTelYTEYEOijRWdIxN3296020;     rXzTelYTEYEOijRWdIxN3296020 = rXzTelYTEYEOijRWdIxN80272361;     rXzTelYTEYEOijRWdIxN80272361 = rXzTelYTEYEOijRWdIxN23176321;     rXzTelYTEYEOijRWdIxN23176321 = rXzTelYTEYEOijRWdIxN70260431;     rXzTelYTEYEOijRWdIxN70260431 = rXzTelYTEYEOijRWdIxN67382846;     rXzTelYTEYEOijRWdIxN67382846 = rXzTelYTEYEOijRWdIxN38719373;     rXzTelYTEYEOijRWdIxN38719373 = rXzTelYTEYEOijRWdIxN99321267;     rXzTelYTEYEOijRWdIxN99321267 = rXzTelYTEYEOijRWdIxN59452097;     rXzTelYTEYEOijRWdIxN59452097 = rXzTelYTEYEOijRWdIxN96506050;     rXzTelYTEYEOijRWdIxN96506050 = rXzTelYTEYEOijRWdIxN44727072;     rXzTelYTEYEOijRWdIxN44727072 = rXzTelYTEYEOijRWdIxN334861;     rXzTelYTEYEOijRWdIxN334861 = rXzTelYTEYEOijRWdIxN60378369;     rXzTelYTEYEOijRWdIxN60378369 = rXzTelYTEYEOijRWdIxN77985468;     rXzTelYTEYEOijRWdIxN77985468 = rXzTelYTEYEOijRWdIxN12130489;     rXzTelYTEYEOijRWdIxN12130489 = rXzTelYTEYEOijRWdIxN14848684;     rXzTelYTEYEOijRWdIxN14848684 = rXzTelYTEYEOijRWdIxN31148597;     rXzTelYTEYEOijRWdIxN31148597 = rXzTelYTEYEOijRWdIxN53080733;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void DGrRvlZNcdpqBuUIOreB91690343() {     float ydNdThoqVIBhIogEPKEb53074261 = -495389283;    float ydNdThoqVIBhIogEPKEb41995954 = -993797304;    float ydNdThoqVIBhIogEPKEb74834126 = -544148828;    float ydNdThoqVIBhIogEPKEb82443886 = -508496036;    float ydNdThoqVIBhIogEPKEb22127468 = -799345933;    float ydNdThoqVIBhIogEPKEb31274247 = -884873551;    float ydNdThoqVIBhIogEPKEb74454151 = -716944683;    float ydNdThoqVIBhIogEPKEb49878779 = -798601591;    float ydNdThoqVIBhIogEPKEb59562361 = -341258551;    float ydNdThoqVIBhIogEPKEb44710051 = -881660046;    float ydNdThoqVIBhIogEPKEb86301955 = -466569368;    float ydNdThoqVIBhIogEPKEb70350803 = -562787913;    float ydNdThoqVIBhIogEPKEb61080283 = -993445887;    float ydNdThoqVIBhIogEPKEb23122219 = -718950084;    float ydNdThoqVIBhIogEPKEb30916071 = -474636697;    float ydNdThoqVIBhIogEPKEb14659928 = -846566676;    float ydNdThoqVIBhIogEPKEb24779847 = -23726056;    float ydNdThoqVIBhIogEPKEb96433476 = -123049909;    float ydNdThoqVIBhIogEPKEb13761873 = -994844867;    float ydNdThoqVIBhIogEPKEb55222339 = -253882858;    float ydNdThoqVIBhIogEPKEb29039352 = -952991392;    float ydNdThoqVIBhIogEPKEb18469645 = -786376900;    float ydNdThoqVIBhIogEPKEb90034880 = -812157779;    float ydNdThoqVIBhIogEPKEb45266770 = -12669630;    float ydNdThoqVIBhIogEPKEb83825230 = 78483144;    float ydNdThoqVIBhIogEPKEb66341989 = -366049663;    float ydNdThoqVIBhIogEPKEb23952117 = -927531203;    float ydNdThoqVIBhIogEPKEb34441806 = -283914181;    float ydNdThoqVIBhIogEPKEb79879766 = 3111307;    float ydNdThoqVIBhIogEPKEb92967192 = -64675044;    float ydNdThoqVIBhIogEPKEb77743438 = -507801828;    float ydNdThoqVIBhIogEPKEb49576616 = -776178949;    float ydNdThoqVIBhIogEPKEb5467938 = -985031491;    float ydNdThoqVIBhIogEPKEb3327355 = -456582106;    float ydNdThoqVIBhIogEPKEb33984764 = -700855421;    float ydNdThoqVIBhIogEPKEb92416611 = -917925732;    float ydNdThoqVIBhIogEPKEb43297718 = -428741061;    float ydNdThoqVIBhIogEPKEb8405215 = -927642116;    float ydNdThoqVIBhIogEPKEb43560122 = -207615021;    float ydNdThoqVIBhIogEPKEb14946034 = -387734731;    float ydNdThoqVIBhIogEPKEb55102571 = -192539714;    float ydNdThoqVIBhIogEPKEb77146538 = -844428280;    float ydNdThoqVIBhIogEPKEb32358723 = -58653030;    float ydNdThoqVIBhIogEPKEb77820360 = -670899962;    float ydNdThoqVIBhIogEPKEb16295242 = -440249007;    float ydNdThoqVIBhIogEPKEb10756567 = -992201177;    float ydNdThoqVIBhIogEPKEb58316810 = -855520141;    float ydNdThoqVIBhIogEPKEb30578470 = -73397768;    float ydNdThoqVIBhIogEPKEb83181638 = -559842229;    float ydNdThoqVIBhIogEPKEb89603489 = 30429605;    float ydNdThoqVIBhIogEPKEb26840673 = -710281458;    float ydNdThoqVIBhIogEPKEb78605655 = -698327311;    float ydNdThoqVIBhIogEPKEb17124786 = -281756038;    float ydNdThoqVIBhIogEPKEb60005354 = -526725929;    float ydNdThoqVIBhIogEPKEb62650991 = -171739693;    float ydNdThoqVIBhIogEPKEb34604616 = -709012384;    float ydNdThoqVIBhIogEPKEb51961074 = -81639525;    float ydNdThoqVIBhIogEPKEb29567357 = -431479198;    float ydNdThoqVIBhIogEPKEb98618655 = -486979181;    float ydNdThoqVIBhIogEPKEb55785479 = -333296270;    float ydNdThoqVIBhIogEPKEb7322131 = -957342348;    float ydNdThoqVIBhIogEPKEb40012345 = -333030502;    float ydNdThoqVIBhIogEPKEb69999012 = -701712899;    float ydNdThoqVIBhIogEPKEb66595168 = -176583508;    float ydNdThoqVIBhIogEPKEb66966613 = -273858218;    float ydNdThoqVIBhIogEPKEb36725340 = -690390419;    float ydNdThoqVIBhIogEPKEb64882865 = -577756423;    float ydNdThoqVIBhIogEPKEb57752929 = -436863782;    float ydNdThoqVIBhIogEPKEb89137454 = 81905337;    float ydNdThoqVIBhIogEPKEb38499460 = -556710965;    float ydNdThoqVIBhIogEPKEb71362210 = -317825616;    float ydNdThoqVIBhIogEPKEb16374632 = -96083941;    float ydNdThoqVIBhIogEPKEb52873354 = -915434889;    float ydNdThoqVIBhIogEPKEb98815839 = -507110137;    float ydNdThoqVIBhIogEPKEb119768 = 38656856;    float ydNdThoqVIBhIogEPKEb51892814 = -8563113;    float ydNdThoqVIBhIogEPKEb86110922 = -627723870;    float ydNdThoqVIBhIogEPKEb12214521 = -41257818;    float ydNdThoqVIBhIogEPKEb28971528 = -572420624;    float ydNdThoqVIBhIogEPKEb73068663 = 70684320;    float ydNdThoqVIBhIogEPKEb8025179 = -510529522;    float ydNdThoqVIBhIogEPKEb93373646 = -754133436;    float ydNdThoqVIBhIogEPKEb51260168 = -724071952;    float ydNdThoqVIBhIogEPKEb90276277 = 72681702;    float ydNdThoqVIBhIogEPKEb66126520 = -354393587;    float ydNdThoqVIBhIogEPKEb99137782 = -809474517;    float ydNdThoqVIBhIogEPKEb32451830 = -394422912;    float ydNdThoqVIBhIogEPKEb45462584 = -358305562;    float ydNdThoqVIBhIogEPKEb40676363 = -184842414;    float ydNdThoqVIBhIogEPKEb99380147 = -991843037;    float ydNdThoqVIBhIogEPKEb40455537 = -736286208;    float ydNdThoqVIBhIogEPKEb13730362 = -997261863;    float ydNdThoqVIBhIogEPKEb9786559 = -340662936;    float ydNdThoqVIBhIogEPKEb87774643 = -874318751;    float ydNdThoqVIBhIogEPKEb7623903 = -430392383;    float ydNdThoqVIBhIogEPKEb15090226 = -859509213;    float ydNdThoqVIBhIogEPKEb7147526 = -42715382;    float ydNdThoqVIBhIogEPKEb65763554 = -882069523;    float ydNdThoqVIBhIogEPKEb10853747 = -297041745;    float ydNdThoqVIBhIogEPKEb79569901 = -495389283;     ydNdThoqVIBhIogEPKEb53074261 = ydNdThoqVIBhIogEPKEb41995954;     ydNdThoqVIBhIogEPKEb41995954 = ydNdThoqVIBhIogEPKEb74834126;     ydNdThoqVIBhIogEPKEb74834126 = ydNdThoqVIBhIogEPKEb82443886;     ydNdThoqVIBhIogEPKEb82443886 = ydNdThoqVIBhIogEPKEb22127468;     ydNdThoqVIBhIogEPKEb22127468 = ydNdThoqVIBhIogEPKEb31274247;     ydNdThoqVIBhIogEPKEb31274247 = ydNdThoqVIBhIogEPKEb74454151;     ydNdThoqVIBhIogEPKEb74454151 = ydNdThoqVIBhIogEPKEb49878779;     ydNdThoqVIBhIogEPKEb49878779 = ydNdThoqVIBhIogEPKEb59562361;     ydNdThoqVIBhIogEPKEb59562361 = ydNdThoqVIBhIogEPKEb44710051;     ydNdThoqVIBhIogEPKEb44710051 = ydNdThoqVIBhIogEPKEb86301955;     ydNdThoqVIBhIogEPKEb86301955 = ydNdThoqVIBhIogEPKEb70350803;     ydNdThoqVIBhIogEPKEb70350803 = ydNdThoqVIBhIogEPKEb61080283;     ydNdThoqVIBhIogEPKEb61080283 = ydNdThoqVIBhIogEPKEb23122219;     ydNdThoqVIBhIogEPKEb23122219 = ydNdThoqVIBhIogEPKEb30916071;     ydNdThoqVIBhIogEPKEb30916071 = ydNdThoqVIBhIogEPKEb14659928;     ydNdThoqVIBhIogEPKEb14659928 = ydNdThoqVIBhIogEPKEb24779847;     ydNdThoqVIBhIogEPKEb24779847 = ydNdThoqVIBhIogEPKEb96433476;     ydNdThoqVIBhIogEPKEb96433476 = ydNdThoqVIBhIogEPKEb13761873;     ydNdThoqVIBhIogEPKEb13761873 = ydNdThoqVIBhIogEPKEb55222339;     ydNdThoqVIBhIogEPKEb55222339 = ydNdThoqVIBhIogEPKEb29039352;     ydNdThoqVIBhIogEPKEb29039352 = ydNdThoqVIBhIogEPKEb18469645;     ydNdThoqVIBhIogEPKEb18469645 = ydNdThoqVIBhIogEPKEb90034880;     ydNdThoqVIBhIogEPKEb90034880 = ydNdThoqVIBhIogEPKEb45266770;     ydNdThoqVIBhIogEPKEb45266770 = ydNdThoqVIBhIogEPKEb83825230;     ydNdThoqVIBhIogEPKEb83825230 = ydNdThoqVIBhIogEPKEb66341989;     ydNdThoqVIBhIogEPKEb66341989 = ydNdThoqVIBhIogEPKEb23952117;     ydNdThoqVIBhIogEPKEb23952117 = ydNdThoqVIBhIogEPKEb34441806;     ydNdThoqVIBhIogEPKEb34441806 = ydNdThoqVIBhIogEPKEb79879766;     ydNdThoqVIBhIogEPKEb79879766 = ydNdThoqVIBhIogEPKEb92967192;     ydNdThoqVIBhIogEPKEb92967192 = ydNdThoqVIBhIogEPKEb77743438;     ydNdThoqVIBhIogEPKEb77743438 = ydNdThoqVIBhIogEPKEb49576616;     ydNdThoqVIBhIogEPKEb49576616 = ydNdThoqVIBhIogEPKEb5467938;     ydNdThoqVIBhIogEPKEb5467938 = ydNdThoqVIBhIogEPKEb3327355;     ydNdThoqVIBhIogEPKEb3327355 = ydNdThoqVIBhIogEPKEb33984764;     ydNdThoqVIBhIogEPKEb33984764 = ydNdThoqVIBhIogEPKEb92416611;     ydNdThoqVIBhIogEPKEb92416611 = ydNdThoqVIBhIogEPKEb43297718;     ydNdThoqVIBhIogEPKEb43297718 = ydNdThoqVIBhIogEPKEb8405215;     ydNdThoqVIBhIogEPKEb8405215 = ydNdThoqVIBhIogEPKEb43560122;     ydNdThoqVIBhIogEPKEb43560122 = ydNdThoqVIBhIogEPKEb14946034;     ydNdThoqVIBhIogEPKEb14946034 = ydNdThoqVIBhIogEPKEb55102571;     ydNdThoqVIBhIogEPKEb55102571 = ydNdThoqVIBhIogEPKEb77146538;     ydNdThoqVIBhIogEPKEb77146538 = ydNdThoqVIBhIogEPKEb32358723;     ydNdThoqVIBhIogEPKEb32358723 = ydNdThoqVIBhIogEPKEb77820360;     ydNdThoqVIBhIogEPKEb77820360 = ydNdThoqVIBhIogEPKEb16295242;     ydNdThoqVIBhIogEPKEb16295242 = ydNdThoqVIBhIogEPKEb10756567;     ydNdThoqVIBhIogEPKEb10756567 = ydNdThoqVIBhIogEPKEb58316810;     ydNdThoqVIBhIogEPKEb58316810 = ydNdThoqVIBhIogEPKEb30578470;     ydNdThoqVIBhIogEPKEb30578470 = ydNdThoqVIBhIogEPKEb83181638;     ydNdThoqVIBhIogEPKEb83181638 = ydNdThoqVIBhIogEPKEb89603489;     ydNdThoqVIBhIogEPKEb89603489 = ydNdThoqVIBhIogEPKEb26840673;     ydNdThoqVIBhIogEPKEb26840673 = ydNdThoqVIBhIogEPKEb78605655;     ydNdThoqVIBhIogEPKEb78605655 = ydNdThoqVIBhIogEPKEb17124786;     ydNdThoqVIBhIogEPKEb17124786 = ydNdThoqVIBhIogEPKEb60005354;     ydNdThoqVIBhIogEPKEb60005354 = ydNdThoqVIBhIogEPKEb62650991;     ydNdThoqVIBhIogEPKEb62650991 = ydNdThoqVIBhIogEPKEb34604616;     ydNdThoqVIBhIogEPKEb34604616 = ydNdThoqVIBhIogEPKEb51961074;     ydNdThoqVIBhIogEPKEb51961074 = ydNdThoqVIBhIogEPKEb29567357;     ydNdThoqVIBhIogEPKEb29567357 = ydNdThoqVIBhIogEPKEb98618655;     ydNdThoqVIBhIogEPKEb98618655 = ydNdThoqVIBhIogEPKEb55785479;     ydNdThoqVIBhIogEPKEb55785479 = ydNdThoqVIBhIogEPKEb7322131;     ydNdThoqVIBhIogEPKEb7322131 = ydNdThoqVIBhIogEPKEb40012345;     ydNdThoqVIBhIogEPKEb40012345 = ydNdThoqVIBhIogEPKEb69999012;     ydNdThoqVIBhIogEPKEb69999012 = ydNdThoqVIBhIogEPKEb66595168;     ydNdThoqVIBhIogEPKEb66595168 = ydNdThoqVIBhIogEPKEb66966613;     ydNdThoqVIBhIogEPKEb66966613 = ydNdThoqVIBhIogEPKEb36725340;     ydNdThoqVIBhIogEPKEb36725340 = ydNdThoqVIBhIogEPKEb64882865;     ydNdThoqVIBhIogEPKEb64882865 = ydNdThoqVIBhIogEPKEb57752929;     ydNdThoqVIBhIogEPKEb57752929 = ydNdThoqVIBhIogEPKEb89137454;     ydNdThoqVIBhIogEPKEb89137454 = ydNdThoqVIBhIogEPKEb38499460;     ydNdThoqVIBhIogEPKEb38499460 = ydNdThoqVIBhIogEPKEb71362210;     ydNdThoqVIBhIogEPKEb71362210 = ydNdThoqVIBhIogEPKEb16374632;     ydNdThoqVIBhIogEPKEb16374632 = ydNdThoqVIBhIogEPKEb52873354;     ydNdThoqVIBhIogEPKEb52873354 = ydNdThoqVIBhIogEPKEb98815839;     ydNdThoqVIBhIogEPKEb98815839 = ydNdThoqVIBhIogEPKEb119768;     ydNdThoqVIBhIogEPKEb119768 = ydNdThoqVIBhIogEPKEb51892814;     ydNdThoqVIBhIogEPKEb51892814 = ydNdThoqVIBhIogEPKEb86110922;     ydNdThoqVIBhIogEPKEb86110922 = ydNdThoqVIBhIogEPKEb12214521;     ydNdThoqVIBhIogEPKEb12214521 = ydNdThoqVIBhIogEPKEb28971528;     ydNdThoqVIBhIogEPKEb28971528 = ydNdThoqVIBhIogEPKEb73068663;     ydNdThoqVIBhIogEPKEb73068663 = ydNdThoqVIBhIogEPKEb8025179;     ydNdThoqVIBhIogEPKEb8025179 = ydNdThoqVIBhIogEPKEb93373646;     ydNdThoqVIBhIogEPKEb93373646 = ydNdThoqVIBhIogEPKEb51260168;     ydNdThoqVIBhIogEPKEb51260168 = ydNdThoqVIBhIogEPKEb90276277;     ydNdThoqVIBhIogEPKEb90276277 = ydNdThoqVIBhIogEPKEb66126520;     ydNdThoqVIBhIogEPKEb66126520 = ydNdThoqVIBhIogEPKEb99137782;     ydNdThoqVIBhIogEPKEb99137782 = ydNdThoqVIBhIogEPKEb32451830;     ydNdThoqVIBhIogEPKEb32451830 = ydNdThoqVIBhIogEPKEb45462584;     ydNdThoqVIBhIogEPKEb45462584 = ydNdThoqVIBhIogEPKEb40676363;     ydNdThoqVIBhIogEPKEb40676363 = ydNdThoqVIBhIogEPKEb99380147;     ydNdThoqVIBhIogEPKEb99380147 = ydNdThoqVIBhIogEPKEb40455537;     ydNdThoqVIBhIogEPKEb40455537 = ydNdThoqVIBhIogEPKEb13730362;     ydNdThoqVIBhIogEPKEb13730362 = ydNdThoqVIBhIogEPKEb9786559;     ydNdThoqVIBhIogEPKEb9786559 = ydNdThoqVIBhIogEPKEb87774643;     ydNdThoqVIBhIogEPKEb87774643 = ydNdThoqVIBhIogEPKEb7623903;     ydNdThoqVIBhIogEPKEb7623903 = ydNdThoqVIBhIogEPKEb15090226;     ydNdThoqVIBhIogEPKEb15090226 = ydNdThoqVIBhIogEPKEb7147526;     ydNdThoqVIBhIogEPKEb7147526 = ydNdThoqVIBhIogEPKEb65763554;     ydNdThoqVIBhIogEPKEb65763554 = ydNdThoqVIBhIogEPKEb10853747;     ydNdThoqVIBhIogEPKEb10853747 = ydNdThoqVIBhIogEPKEb79569901;     ydNdThoqVIBhIogEPKEb79569901 = ydNdThoqVIBhIogEPKEb53074261;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void lviaZgMnXEsPrpKBrTfI46106557() {     float EysvIlNLjWlhRoeRbLXF23725929 = -13538414;    float EysvIlNLjWlhRoeRbLXF10669472 = -865440603;    float EysvIlNLjWlhRoeRbLXF54803045 = -986443288;    float EysvIlNLjWlhRoeRbLXF22375173 = -698634425;    float EysvIlNLjWlhRoeRbLXF43577789 = -582752280;    float EysvIlNLjWlhRoeRbLXF21274010 = -586454702;    float EysvIlNLjWlhRoeRbLXF92833046 = -715720393;    float EysvIlNLjWlhRoeRbLXF6721160 = -45418681;    float EysvIlNLjWlhRoeRbLXF5427416 = -434605880;    float EysvIlNLjWlhRoeRbLXF67676512 = -82755396;    float EysvIlNLjWlhRoeRbLXF28551117 = -501471297;    float EysvIlNLjWlhRoeRbLXF15897375 = -878551706;    float EysvIlNLjWlhRoeRbLXF40907670 = -902982945;    float EysvIlNLjWlhRoeRbLXF6969253 = -519978294;    float EysvIlNLjWlhRoeRbLXF13636090 = -624386700;    float EysvIlNLjWlhRoeRbLXF47188568 = -416550263;    float EysvIlNLjWlhRoeRbLXF18632673 = 67088814;    float EysvIlNLjWlhRoeRbLXF16726971 = -112216105;    float EysvIlNLjWlhRoeRbLXF95469654 = -602032810;    float EysvIlNLjWlhRoeRbLXF14009048 = -796249163;    float EysvIlNLjWlhRoeRbLXF84175106 = -861670533;    float EysvIlNLjWlhRoeRbLXF79865409 = -17662205;    float EysvIlNLjWlhRoeRbLXF91978688 = -280000924;    float EysvIlNLjWlhRoeRbLXF79116306 = -705070536;    float EysvIlNLjWlhRoeRbLXF47949619 = -697421544;    float EysvIlNLjWlhRoeRbLXF96939569 = -464334108;    float EysvIlNLjWlhRoeRbLXF32212902 = -222708978;    float EysvIlNLjWlhRoeRbLXF42224032 = -789037949;    float EysvIlNLjWlhRoeRbLXF12022254 = -291246910;    float EysvIlNLjWlhRoeRbLXF32139288 = -623070114;    float EysvIlNLjWlhRoeRbLXF84815434 = -256235514;    float EysvIlNLjWlhRoeRbLXF94088076 = -4476112;    float EysvIlNLjWlhRoeRbLXF88413298 = -931993100;    float EysvIlNLjWlhRoeRbLXF68692329 = -20111790;    float EysvIlNLjWlhRoeRbLXF12009001 = -547802200;    float EysvIlNLjWlhRoeRbLXF52409870 = -997152280;    float EysvIlNLjWlhRoeRbLXF87089204 = -617334268;    float EysvIlNLjWlhRoeRbLXF61197703 = -711755223;    float EysvIlNLjWlhRoeRbLXF22093411 = -163343657;    float EysvIlNLjWlhRoeRbLXF10049332 = 94656906;    float EysvIlNLjWlhRoeRbLXF15175855 = -576558593;    float EysvIlNLjWlhRoeRbLXF5629530 = -613817350;    float EysvIlNLjWlhRoeRbLXF81325682 = -45407082;    float EysvIlNLjWlhRoeRbLXF84328660 = -742874934;    float EysvIlNLjWlhRoeRbLXF12249300 = -748895401;    float EysvIlNLjWlhRoeRbLXF17162402 = -750538332;    float EysvIlNLjWlhRoeRbLXF84448013 = -971589940;    float EysvIlNLjWlhRoeRbLXF96192680 = -525528063;    float EysvIlNLjWlhRoeRbLXF88623090 = -545606198;    float EysvIlNLjWlhRoeRbLXF98030975 = -252579274;    float EysvIlNLjWlhRoeRbLXF95029996 = -256853579;    float EysvIlNLjWlhRoeRbLXF20477160 = -667547033;    float EysvIlNLjWlhRoeRbLXF72200443 = -209554080;    float EysvIlNLjWlhRoeRbLXF50232658 = -87607426;    float EysvIlNLjWlhRoeRbLXF61931450 = -282680607;    float EysvIlNLjWlhRoeRbLXF43860519 = -995876209;    float EysvIlNLjWlhRoeRbLXF18690783 = -485439680;    float EysvIlNLjWlhRoeRbLXF75686739 = -181372752;    float EysvIlNLjWlhRoeRbLXF74425554 = 98787118;    float EysvIlNLjWlhRoeRbLXF46638220 = -18418173;    float EysvIlNLjWlhRoeRbLXF89061107 = -263745724;    float EysvIlNLjWlhRoeRbLXF50609015 = -926682445;    float EysvIlNLjWlhRoeRbLXF94698905 = -754171772;    float EysvIlNLjWlhRoeRbLXF73288127 = -811535766;    float EysvIlNLjWlhRoeRbLXF82861077 = -826519883;    float EysvIlNLjWlhRoeRbLXF34463040 = -396995186;    float EysvIlNLjWlhRoeRbLXF27484076 = -946558607;    float EysvIlNLjWlhRoeRbLXF72215340 = -782871156;    float EysvIlNLjWlhRoeRbLXF94960251 = -972176094;    float EysvIlNLjWlhRoeRbLXF61226220 = -627234420;    float EysvIlNLjWlhRoeRbLXF60099364 = -799215996;    float EysvIlNLjWlhRoeRbLXF57434970 = -221155963;    float EysvIlNLjWlhRoeRbLXF94633559 = -948872449;    float EysvIlNLjWlhRoeRbLXF85420323 = -596689716;    float EysvIlNLjWlhRoeRbLXF98833192 = -119690571;    float EysvIlNLjWlhRoeRbLXF78545577 = -147853184;    float EysvIlNLjWlhRoeRbLXF98539727 = -972255124;    float EysvIlNLjWlhRoeRbLXF7650029 = -537125991;    float EysvIlNLjWlhRoeRbLXF66867006 = -956175136;    float EysvIlNLjWlhRoeRbLXF30787217 = -946883212;    float EysvIlNLjWlhRoeRbLXF12491556 = -492744168;    float EysvIlNLjWlhRoeRbLXF36020222 = -697180916;    float EysvIlNLjWlhRoeRbLXF53600941 = -143431751;    float EysvIlNLjWlhRoeRbLXF13991279 = 61332364;    float EysvIlNLjWlhRoeRbLXF37109292 = -266216536;    float EysvIlNLjWlhRoeRbLXF64338274 = -588688481;    float EysvIlNLjWlhRoeRbLXF21887634 = -794922033;    float EysvIlNLjWlhRoeRbLXF38180640 = -744385675;    float EysvIlNLjWlhRoeRbLXF6760880 = -737431184;    float EysvIlNLjWlhRoeRbLXF68148482 = -551925992;    float EysvIlNLjWlhRoeRbLXF33719088 = -411712601;    float EysvIlNLjWlhRoeRbLXF11402465 = -335961517;    float EysvIlNLjWlhRoeRbLXF86772149 = -710542342;    float EysvIlNLjWlhRoeRbLXF75455191 = -44925485;    float EysvIlNLjWlhRoeRbLXF20988224 = -641597370;    float EysvIlNLjWlhRoeRbLXF64566840 = -649876149;    float EysvIlNLjWlhRoeRbLXF10930624 = -859645578;    float EysvIlNLjWlhRoeRbLXF8037556 = -233871317;    float EysvIlNLjWlhRoeRbLXF1467583 = -916355051;    float EysvIlNLjWlhRoeRbLXF77786260 = -13538414;     EysvIlNLjWlhRoeRbLXF23725929 = EysvIlNLjWlhRoeRbLXF10669472;     EysvIlNLjWlhRoeRbLXF10669472 = EysvIlNLjWlhRoeRbLXF54803045;     EysvIlNLjWlhRoeRbLXF54803045 = EysvIlNLjWlhRoeRbLXF22375173;     EysvIlNLjWlhRoeRbLXF22375173 = EysvIlNLjWlhRoeRbLXF43577789;     EysvIlNLjWlhRoeRbLXF43577789 = EysvIlNLjWlhRoeRbLXF21274010;     EysvIlNLjWlhRoeRbLXF21274010 = EysvIlNLjWlhRoeRbLXF92833046;     EysvIlNLjWlhRoeRbLXF92833046 = EysvIlNLjWlhRoeRbLXF6721160;     EysvIlNLjWlhRoeRbLXF6721160 = EysvIlNLjWlhRoeRbLXF5427416;     EysvIlNLjWlhRoeRbLXF5427416 = EysvIlNLjWlhRoeRbLXF67676512;     EysvIlNLjWlhRoeRbLXF67676512 = EysvIlNLjWlhRoeRbLXF28551117;     EysvIlNLjWlhRoeRbLXF28551117 = EysvIlNLjWlhRoeRbLXF15897375;     EysvIlNLjWlhRoeRbLXF15897375 = EysvIlNLjWlhRoeRbLXF40907670;     EysvIlNLjWlhRoeRbLXF40907670 = EysvIlNLjWlhRoeRbLXF6969253;     EysvIlNLjWlhRoeRbLXF6969253 = EysvIlNLjWlhRoeRbLXF13636090;     EysvIlNLjWlhRoeRbLXF13636090 = EysvIlNLjWlhRoeRbLXF47188568;     EysvIlNLjWlhRoeRbLXF47188568 = EysvIlNLjWlhRoeRbLXF18632673;     EysvIlNLjWlhRoeRbLXF18632673 = EysvIlNLjWlhRoeRbLXF16726971;     EysvIlNLjWlhRoeRbLXF16726971 = EysvIlNLjWlhRoeRbLXF95469654;     EysvIlNLjWlhRoeRbLXF95469654 = EysvIlNLjWlhRoeRbLXF14009048;     EysvIlNLjWlhRoeRbLXF14009048 = EysvIlNLjWlhRoeRbLXF84175106;     EysvIlNLjWlhRoeRbLXF84175106 = EysvIlNLjWlhRoeRbLXF79865409;     EysvIlNLjWlhRoeRbLXF79865409 = EysvIlNLjWlhRoeRbLXF91978688;     EysvIlNLjWlhRoeRbLXF91978688 = EysvIlNLjWlhRoeRbLXF79116306;     EysvIlNLjWlhRoeRbLXF79116306 = EysvIlNLjWlhRoeRbLXF47949619;     EysvIlNLjWlhRoeRbLXF47949619 = EysvIlNLjWlhRoeRbLXF96939569;     EysvIlNLjWlhRoeRbLXF96939569 = EysvIlNLjWlhRoeRbLXF32212902;     EysvIlNLjWlhRoeRbLXF32212902 = EysvIlNLjWlhRoeRbLXF42224032;     EysvIlNLjWlhRoeRbLXF42224032 = EysvIlNLjWlhRoeRbLXF12022254;     EysvIlNLjWlhRoeRbLXF12022254 = EysvIlNLjWlhRoeRbLXF32139288;     EysvIlNLjWlhRoeRbLXF32139288 = EysvIlNLjWlhRoeRbLXF84815434;     EysvIlNLjWlhRoeRbLXF84815434 = EysvIlNLjWlhRoeRbLXF94088076;     EysvIlNLjWlhRoeRbLXF94088076 = EysvIlNLjWlhRoeRbLXF88413298;     EysvIlNLjWlhRoeRbLXF88413298 = EysvIlNLjWlhRoeRbLXF68692329;     EysvIlNLjWlhRoeRbLXF68692329 = EysvIlNLjWlhRoeRbLXF12009001;     EysvIlNLjWlhRoeRbLXF12009001 = EysvIlNLjWlhRoeRbLXF52409870;     EysvIlNLjWlhRoeRbLXF52409870 = EysvIlNLjWlhRoeRbLXF87089204;     EysvIlNLjWlhRoeRbLXF87089204 = EysvIlNLjWlhRoeRbLXF61197703;     EysvIlNLjWlhRoeRbLXF61197703 = EysvIlNLjWlhRoeRbLXF22093411;     EysvIlNLjWlhRoeRbLXF22093411 = EysvIlNLjWlhRoeRbLXF10049332;     EysvIlNLjWlhRoeRbLXF10049332 = EysvIlNLjWlhRoeRbLXF15175855;     EysvIlNLjWlhRoeRbLXF15175855 = EysvIlNLjWlhRoeRbLXF5629530;     EysvIlNLjWlhRoeRbLXF5629530 = EysvIlNLjWlhRoeRbLXF81325682;     EysvIlNLjWlhRoeRbLXF81325682 = EysvIlNLjWlhRoeRbLXF84328660;     EysvIlNLjWlhRoeRbLXF84328660 = EysvIlNLjWlhRoeRbLXF12249300;     EysvIlNLjWlhRoeRbLXF12249300 = EysvIlNLjWlhRoeRbLXF17162402;     EysvIlNLjWlhRoeRbLXF17162402 = EysvIlNLjWlhRoeRbLXF84448013;     EysvIlNLjWlhRoeRbLXF84448013 = EysvIlNLjWlhRoeRbLXF96192680;     EysvIlNLjWlhRoeRbLXF96192680 = EysvIlNLjWlhRoeRbLXF88623090;     EysvIlNLjWlhRoeRbLXF88623090 = EysvIlNLjWlhRoeRbLXF98030975;     EysvIlNLjWlhRoeRbLXF98030975 = EysvIlNLjWlhRoeRbLXF95029996;     EysvIlNLjWlhRoeRbLXF95029996 = EysvIlNLjWlhRoeRbLXF20477160;     EysvIlNLjWlhRoeRbLXF20477160 = EysvIlNLjWlhRoeRbLXF72200443;     EysvIlNLjWlhRoeRbLXF72200443 = EysvIlNLjWlhRoeRbLXF50232658;     EysvIlNLjWlhRoeRbLXF50232658 = EysvIlNLjWlhRoeRbLXF61931450;     EysvIlNLjWlhRoeRbLXF61931450 = EysvIlNLjWlhRoeRbLXF43860519;     EysvIlNLjWlhRoeRbLXF43860519 = EysvIlNLjWlhRoeRbLXF18690783;     EysvIlNLjWlhRoeRbLXF18690783 = EysvIlNLjWlhRoeRbLXF75686739;     EysvIlNLjWlhRoeRbLXF75686739 = EysvIlNLjWlhRoeRbLXF74425554;     EysvIlNLjWlhRoeRbLXF74425554 = EysvIlNLjWlhRoeRbLXF46638220;     EysvIlNLjWlhRoeRbLXF46638220 = EysvIlNLjWlhRoeRbLXF89061107;     EysvIlNLjWlhRoeRbLXF89061107 = EysvIlNLjWlhRoeRbLXF50609015;     EysvIlNLjWlhRoeRbLXF50609015 = EysvIlNLjWlhRoeRbLXF94698905;     EysvIlNLjWlhRoeRbLXF94698905 = EysvIlNLjWlhRoeRbLXF73288127;     EysvIlNLjWlhRoeRbLXF73288127 = EysvIlNLjWlhRoeRbLXF82861077;     EysvIlNLjWlhRoeRbLXF82861077 = EysvIlNLjWlhRoeRbLXF34463040;     EysvIlNLjWlhRoeRbLXF34463040 = EysvIlNLjWlhRoeRbLXF27484076;     EysvIlNLjWlhRoeRbLXF27484076 = EysvIlNLjWlhRoeRbLXF72215340;     EysvIlNLjWlhRoeRbLXF72215340 = EysvIlNLjWlhRoeRbLXF94960251;     EysvIlNLjWlhRoeRbLXF94960251 = EysvIlNLjWlhRoeRbLXF61226220;     EysvIlNLjWlhRoeRbLXF61226220 = EysvIlNLjWlhRoeRbLXF60099364;     EysvIlNLjWlhRoeRbLXF60099364 = EysvIlNLjWlhRoeRbLXF57434970;     EysvIlNLjWlhRoeRbLXF57434970 = EysvIlNLjWlhRoeRbLXF94633559;     EysvIlNLjWlhRoeRbLXF94633559 = EysvIlNLjWlhRoeRbLXF85420323;     EysvIlNLjWlhRoeRbLXF85420323 = EysvIlNLjWlhRoeRbLXF98833192;     EysvIlNLjWlhRoeRbLXF98833192 = EysvIlNLjWlhRoeRbLXF78545577;     EysvIlNLjWlhRoeRbLXF78545577 = EysvIlNLjWlhRoeRbLXF98539727;     EysvIlNLjWlhRoeRbLXF98539727 = EysvIlNLjWlhRoeRbLXF7650029;     EysvIlNLjWlhRoeRbLXF7650029 = EysvIlNLjWlhRoeRbLXF66867006;     EysvIlNLjWlhRoeRbLXF66867006 = EysvIlNLjWlhRoeRbLXF30787217;     EysvIlNLjWlhRoeRbLXF30787217 = EysvIlNLjWlhRoeRbLXF12491556;     EysvIlNLjWlhRoeRbLXF12491556 = EysvIlNLjWlhRoeRbLXF36020222;     EysvIlNLjWlhRoeRbLXF36020222 = EysvIlNLjWlhRoeRbLXF53600941;     EysvIlNLjWlhRoeRbLXF53600941 = EysvIlNLjWlhRoeRbLXF13991279;     EysvIlNLjWlhRoeRbLXF13991279 = EysvIlNLjWlhRoeRbLXF37109292;     EysvIlNLjWlhRoeRbLXF37109292 = EysvIlNLjWlhRoeRbLXF64338274;     EysvIlNLjWlhRoeRbLXF64338274 = EysvIlNLjWlhRoeRbLXF21887634;     EysvIlNLjWlhRoeRbLXF21887634 = EysvIlNLjWlhRoeRbLXF38180640;     EysvIlNLjWlhRoeRbLXF38180640 = EysvIlNLjWlhRoeRbLXF6760880;     EysvIlNLjWlhRoeRbLXF6760880 = EysvIlNLjWlhRoeRbLXF68148482;     EysvIlNLjWlhRoeRbLXF68148482 = EysvIlNLjWlhRoeRbLXF33719088;     EysvIlNLjWlhRoeRbLXF33719088 = EysvIlNLjWlhRoeRbLXF11402465;     EysvIlNLjWlhRoeRbLXF11402465 = EysvIlNLjWlhRoeRbLXF86772149;     EysvIlNLjWlhRoeRbLXF86772149 = EysvIlNLjWlhRoeRbLXF75455191;     EysvIlNLjWlhRoeRbLXF75455191 = EysvIlNLjWlhRoeRbLXF20988224;     EysvIlNLjWlhRoeRbLXF20988224 = EysvIlNLjWlhRoeRbLXF64566840;     EysvIlNLjWlhRoeRbLXF64566840 = EysvIlNLjWlhRoeRbLXF10930624;     EysvIlNLjWlhRoeRbLXF10930624 = EysvIlNLjWlhRoeRbLXF8037556;     EysvIlNLjWlhRoeRbLXF8037556 = EysvIlNLjWlhRoeRbLXF1467583;     EysvIlNLjWlhRoeRbLXF1467583 = EysvIlNLjWlhRoeRbLXF77786260;     EysvIlNLjWlhRoeRbLXF77786260 = EysvIlNLjWlhRoeRbLXF23725929;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void DSJXnfAKbXmYbOYaHLBS52765301() {     float ilmiDyNAmitfJikLbdEM23719457 = -986062555;    float ilmiDyNAmitfJikLbdEM49241788 = -908035728;    float ilmiDyNAmitfJikLbdEM56116257 = -943571901;    float ilmiDyNAmitfJikLbdEM7455034 = -519175256;    float ilmiDyNAmitfJikLbdEM52417891 = 48892398;    float ilmiDyNAmitfJikLbdEM56079442 = -285887514;    float ilmiDyNAmitfJikLbdEM61380382 = -420256860;    float ilmiDyNAmitfJikLbdEM75935210 = -849595998;    float ilmiDyNAmitfJikLbdEM88167579 = -148251950;    float ilmiDyNAmitfJikLbdEM64316025 = -819562267;    float ilmiDyNAmitfJikLbdEM48871722 = -460943369;    float ilmiDyNAmitfJikLbdEM30189052 = -614030069;    float ilmiDyNAmitfJikLbdEM72760584 = -924641420;    float ilmiDyNAmitfJikLbdEM60813472 = -865131345;    float ilmiDyNAmitfJikLbdEM58808199 = -587099552;    float ilmiDyNAmitfJikLbdEM21753116 = -749345914;    float ilmiDyNAmitfJikLbdEM74485432 = -852525756;    float ilmiDyNAmitfJikLbdEM39727503 = -509287727;    float ilmiDyNAmitfJikLbdEM15897925 = -120015076;    float ilmiDyNAmitfJikLbdEM86082810 = -308273614;    float ilmiDyNAmitfJikLbdEM63844530 = -572136906;    float ilmiDyNAmitfJikLbdEM58161676 = -527654088;    float ilmiDyNAmitfJikLbdEM28857086 = -738958766;    float ilmiDyNAmitfJikLbdEM22766415 = -585991059;    float ilmiDyNAmitfJikLbdEM76392308 = -24115397;    float ilmiDyNAmitfJikLbdEM13513647 = -372540036;    float ilmiDyNAmitfJikLbdEM2434484 = -523508691;    float ilmiDyNAmitfJikLbdEM77627556 = -298004937;    float ilmiDyNAmitfJikLbdEM24700701 = -937317225;    float ilmiDyNAmitfJikLbdEM8229159 = -298501488;    float ilmiDyNAmitfJikLbdEM18228113 = -643328748;    float ilmiDyNAmitfJikLbdEM78811934 = -396297874;    float ilmiDyNAmitfJikLbdEM73865721 = -33937316;    float ilmiDyNAmitfJikLbdEM68975167 = -996793522;    float ilmiDyNAmitfJikLbdEM94367038 = -807116125;    float ilmiDyNAmitfJikLbdEM95238059 = 23081973;    float ilmiDyNAmitfJikLbdEM99030571 = -716987646;    float ilmiDyNAmitfJikLbdEM31115383 = -707017902;    float ilmiDyNAmitfJikLbdEM57407008 = -876025073;    float ilmiDyNAmitfJikLbdEM81922224 = -614774780;    float ilmiDyNAmitfJikLbdEM3031523 = -664502491;    float ilmiDyNAmitfJikLbdEM91327190 = -807609432;    float ilmiDyNAmitfJikLbdEM41609043 = -604537336;    float ilmiDyNAmitfJikLbdEM43560559 = -299839377;    float ilmiDyNAmitfJikLbdEM96267352 = -122154803;    float ilmiDyNAmitfJikLbdEM57590370 = -562583002;    float ilmiDyNAmitfJikLbdEM12145806 = -352870563;    float ilmiDyNAmitfJikLbdEM71576261 = -34289419;    float ilmiDyNAmitfJikLbdEM7281480 = -109370060;    float ilmiDyNAmitfJikLbdEM18076399 = -939604160;    float ilmiDyNAmitfJikLbdEM8289367 = -935408586;    float ilmiDyNAmitfJikLbdEM35024418 = 50307219;    float ilmiDyNAmitfJikLbdEM47648791 = -101608088;    float ilmiDyNAmitfJikLbdEM60482929 = -619527277;    float ilmiDyNAmitfJikLbdEM88920770 = -39579885;    float ilmiDyNAmitfJikLbdEM65557781 = -358408467;    float ilmiDyNAmitfJikLbdEM20384703 = -69076963;    float ilmiDyNAmitfJikLbdEM33349843 = -257580843;    float ilmiDyNAmitfJikLbdEM31062725 = -395059859;    float ilmiDyNAmitfJikLbdEM38904245 = -578567566;    float ilmiDyNAmitfJikLbdEM53644958 = -762378823;    float ilmiDyNAmitfJikLbdEM83752826 = -22251923;    float ilmiDyNAmitfJikLbdEM51234509 = -912278774;    float ilmiDyNAmitfJikLbdEM79938421 = -849750462;    float ilmiDyNAmitfJikLbdEM46087912 = -76233520;    float ilmiDyNAmitfJikLbdEM70059788 = 35354504;    float ilmiDyNAmitfJikLbdEM56323330 = -480092753;    float ilmiDyNAmitfJikLbdEM3785418 = -927847898;    float ilmiDyNAmitfJikLbdEM66446434 = 41984779;    float ilmiDyNAmitfJikLbdEM63570139 = -510181526;    float ilmiDyNAmitfJikLbdEM22722544 = 67641732;    float ilmiDyNAmitfJikLbdEM43370050 = -45507854;    float ilmiDyNAmitfJikLbdEM82320495 = -633262654;    float ilmiDyNAmitfJikLbdEM33975700 = -505240297;    float ilmiDyNAmitfJikLbdEM83051288 = -643771124;    float ilmiDyNAmitfJikLbdEM72517340 = -764527475;    float ilmiDyNAmitfJikLbdEM16552633 = -923116753;    float ilmiDyNAmitfJikLbdEM85296526 = -339119389;    float ilmiDyNAmitfJikLbdEM26499063 = -363836257;    float ilmiDyNAmitfJikLbdEM18801939 = -461532395;    float ilmiDyNAmitfJikLbdEM1367841 = 80330526;    float ilmiDyNAmitfJikLbdEM30858223 = -389219272;    float ilmiDyNAmitfJikLbdEM70346076 = -88634878;    float ilmiDyNAmitfJikLbdEM6624303 = -997713065;    float ilmiDyNAmitfJikLbdEM99939791 = -363092903;    float ilmiDyNAmitfJikLbdEM83203695 = -593635967;    float ilmiDyNAmitfJikLbdEM31163143 = -194689786;    float ilmiDyNAmitfJikLbdEM13382793 = -414410039;    float ilmiDyNAmitfJikLbdEM80054396 = -857213638;    float ilmiDyNAmitfJikLbdEM28809257 = -348707658;    float ilmiDyNAmitfJikLbdEM74853357 = -907841064;    float ilmiDyNAmitfJikLbdEM65680729 = -359406804;    float ilmiDyNAmitfJikLbdEM52658 = -211958044;    float ilmiDyNAmitfJikLbdEM18502763 = -197457508;    float ilmiDyNAmitfJikLbdEM28277266 = -852395957;    float ilmiDyNAmitfJikLbdEM19278697 = -542250568;    float ilmiDyNAmitfJikLbdEM40092681 = -895330658;    float ilmiDyNAmitfJikLbdEM61670621 = -754786875;    float ilmiDyNAmitfJikLbdEM97472646 = -123605858;    float ilmiDyNAmitfJikLbdEM26207565 = -986062555;     ilmiDyNAmitfJikLbdEM23719457 = ilmiDyNAmitfJikLbdEM49241788;     ilmiDyNAmitfJikLbdEM49241788 = ilmiDyNAmitfJikLbdEM56116257;     ilmiDyNAmitfJikLbdEM56116257 = ilmiDyNAmitfJikLbdEM7455034;     ilmiDyNAmitfJikLbdEM7455034 = ilmiDyNAmitfJikLbdEM52417891;     ilmiDyNAmitfJikLbdEM52417891 = ilmiDyNAmitfJikLbdEM56079442;     ilmiDyNAmitfJikLbdEM56079442 = ilmiDyNAmitfJikLbdEM61380382;     ilmiDyNAmitfJikLbdEM61380382 = ilmiDyNAmitfJikLbdEM75935210;     ilmiDyNAmitfJikLbdEM75935210 = ilmiDyNAmitfJikLbdEM88167579;     ilmiDyNAmitfJikLbdEM88167579 = ilmiDyNAmitfJikLbdEM64316025;     ilmiDyNAmitfJikLbdEM64316025 = ilmiDyNAmitfJikLbdEM48871722;     ilmiDyNAmitfJikLbdEM48871722 = ilmiDyNAmitfJikLbdEM30189052;     ilmiDyNAmitfJikLbdEM30189052 = ilmiDyNAmitfJikLbdEM72760584;     ilmiDyNAmitfJikLbdEM72760584 = ilmiDyNAmitfJikLbdEM60813472;     ilmiDyNAmitfJikLbdEM60813472 = ilmiDyNAmitfJikLbdEM58808199;     ilmiDyNAmitfJikLbdEM58808199 = ilmiDyNAmitfJikLbdEM21753116;     ilmiDyNAmitfJikLbdEM21753116 = ilmiDyNAmitfJikLbdEM74485432;     ilmiDyNAmitfJikLbdEM74485432 = ilmiDyNAmitfJikLbdEM39727503;     ilmiDyNAmitfJikLbdEM39727503 = ilmiDyNAmitfJikLbdEM15897925;     ilmiDyNAmitfJikLbdEM15897925 = ilmiDyNAmitfJikLbdEM86082810;     ilmiDyNAmitfJikLbdEM86082810 = ilmiDyNAmitfJikLbdEM63844530;     ilmiDyNAmitfJikLbdEM63844530 = ilmiDyNAmitfJikLbdEM58161676;     ilmiDyNAmitfJikLbdEM58161676 = ilmiDyNAmitfJikLbdEM28857086;     ilmiDyNAmitfJikLbdEM28857086 = ilmiDyNAmitfJikLbdEM22766415;     ilmiDyNAmitfJikLbdEM22766415 = ilmiDyNAmitfJikLbdEM76392308;     ilmiDyNAmitfJikLbdEM76392308 = ilmiDyNAmitfJikLbdEM13513647;     ilmiDyNAmitfJikLbdEM13513647 = ilmiDyNAmitfJikLbdEM2434484;     ilmiDyNAmitfJikLbdEM2434484 = ilmiDyNAmitfJikLbdEM77627556;     ilmiDyNAmitfJikLbdEM77627556 = ilmiDyNAmitfJikLbdEM24700701;     ilmiDyNAmitfJikLbdEM24700701 = ilmiDyNAmitfJikLbdEM8229159;     ilmiDyNAmitfJikLbdEM8229159 = ilmiDyNAmitfJikLbdEM18228113;     ilmiDyNAmitfJikLbdEM18228113 = ilmiDyNAmitfJikLbdEM78811934;     ilmiDyNAmitfJikLbdEM78811934 = ilmiDyNAmitfJikLbdEM73865721;     ilmiDyNAmitfJikLbdEM73865721 = ilmiDyNAmitfJikLbdEM68975167;     ilmiDyNAmitfJikLbdEM68975167 = ilmiDyNAmitfJikLbdEM94367038;     ilmiDyNAmitfJikLbdEM94367038 = ilmiDyNAmitfJikLbdEM95238059;     ilmiDyNAmitfJikLbdEM95238059 = ilmiDyNAmitfJikLbdEM99030571;     ilmiDyNAmitfJikLbdEM99030571 = ilmiDyNAmitfJikLbdEM31115383;     ilmiDyNAmitfJikLbdEM31115383 = ilmiDyNAmitfJikLbdEM57407008;     ilmiDyNAmitfJikLbdEM57407008 = ilmiDyNAmitfJikLbdEM81922224;     ilmiDyNAmitfJikLbdEM81922224 = ilmiDyNAmitfJikLbdEM3031523;     ilmiDyNAmitfJikLbdEM3031523 = ilmiDyNAmitfJikLbdEM91327190;     ilmiDyNAmitfJikLbdEM91327190 = ilmiDyNAmitfJikLbdEM41609043;     ilmiDyNAmitfJikLbdEM41609043 = ilmiDyNAmitfJikLbdEM43560559;     ilmiDyNAmitfJikLbdEM43560559 = ilmiDyNAmitfJikLbdEM96267352;     ilmiDyNAmitfJikLbdEM96267352 = ilmiDyNAmitfJikLbdEM57590370;     ilmiDyNAmitfJikLbdEM57590370 = ilmiDyNAmitfJikLbdEM12145806;     ilmiDyNAmitfJikLbdEM12145806 = ilmiDyNAmitfJikLbdEM71576261;     ilmiDyNAmitfJikLbdEM71576261 = ilmiDyNAmitfJikLbdEM7281480;     ilmiDyNAmitfJikLbdEM7281480 = ilmiDyNAmitfJikLbdEM18076399;     ilmiDyNAmitfJikLbdEM18076399 = ilmiDyNAmitfJikLbdEM8289367;     ilmiDyNAmitfJikLbdEM8289367 = ilmiDyNAmitfJikLbdEM35024418;     ilmiDyNAmitfJikLbdEM35024418 = ilmiDyNAmitfJikLbdEM47648791;     ilmiDyNAmitfJikLbdEM47648791 = ilmiDyNAmitfJikLbdEM60482929;     ilmiDyNAmitfJikLbdEM60482929 = ilmiDyNAmitfJikLbdEM88920770;     ilmiDyNAmitfJikLbdEM88920770 = ilmiDyNAmitfJikLbdEM65557781;     ilmiDyNAmitfJikLbdEM65557781 = ilmiDyNAmitfJikLbdEM20384703;     ilmiDyNAmitfJikLbdEM20384703 = ilmiDyNAmitfJikLbdEM33349843;     ilmiDyNAmitfJikLbdEM33349843 = ilmiDyNAmitfJikLbdEM31062725;     ilmiDyNAmitfJikLbdEM31062725 = ilmiDyNAmitfJikLbdEM38904245;     ilmiDyNAmitfJikLbdEM38904245 = ilmiDyNAmitfJikLbdEM53644958;     ilmiDyNAmitfJikLbdEM53644958 = ilmiDyNAmitfJikLbdEM83752826;     ilmiDyNAmitfJikLbdEM83752826 = ilmiDyNAmitfJikLbdEM51234509;     ilmiDyNAmitfJikLbdEM51234509 = ilmiDyNAmitfJikLbdEM79938421;     ilmiDyNAmitfJikLbdEM79938421 = ilmiDyNAmitfJikLbdEM46087912;     ilmiDyNAmitfJikLbdEM46087912 = ilmiDyNAmitfJikLbdEM70059788;     ilmiDyNAmitfJikLbdEM70059788 = ilmiDyNAmitfJikLbdEM56323330;     ilmiDyNAmitfJikLbdEM56323330 = ilmiDyNAmitfJikLbdEM3785418;     ilmiDyNAmitfJikLbdEM3785418 = ilmiDyNAmitfJikLbdEM66446434;     ilmiDyNAmitfJikLbdEM66446434 = ilmiDyNAmitfJikLbdEM63570139;     ilmiDyNAmitfJikLbdEM63570139 = ilmiDyNAmitfJikLbdEM22722544;     ilmiDyNAmitfJikLbdEM22722544 = ilmiDyNAmitfJikLbdEM43370050;     ilmiDyNAmitfJikLbdEM43370050 = ilmiDyNAmitfJikLbdEM82320495;     ilmiDyNAmitfJikLbdEM82320495 = ilmiDyNAmitfJikLbdEM33975700;     ilmiDyNAmitfJikLbdEM33975700 = ilmiDyNAmitfJikLbdEM83051288;     ilmiDyNAmitfJikLbdEM83051288 = ilmiDyNAmitfJikLbdEM72517340;     ilmiDyNAmitfJikLbdEM72517340 = ilmiDyNAmitfJikLbdEM16552633;     ilmiDyNAmitfJikLbdEM16552633 = ilmiDyNAmitfJikLbdEM85296526;     ilmiDyNAmitfJikLbdEM85296526 = ilmiDyNAmitfJikLbdEM26499063;     ilmiDyNAmitfJikLbdEM26499063 = ilmiDyNAmitfJikLbdEM18801939;     ilmiDyNAmitfJikLbdEM18801939 = ilmiDyNAmitfJikLbdEM1367841;     ilmiDyNAmitfJikLbdEM1367841 = ilmiDyNAmitfJikLbdEM30858223;     ilmiDyNAmitfJikLbdEM30858223 = ilmiDyNAmitfJikLbdEM70346076;     ilmiDyNAmitfJikLbdEM70346076 = ilmiDyNAmitfJikLbdEM6624303;     ilmiDyNAmitfJikLbdEM6624303 = ilmiDyNAmitfJikLbdEM99939791;     ilmiDyNAmitfJikLbdEM99939791 = ilmiDyNAmitfJikLbdEM83203695;     ilmiDyNAmitfJikLbdEM83203695 = ilmiDyNAmitfJikLbdEM31163143;     ilmiDyNAmitfJikLbdEM31163143 = ilmiDyNAmitfJikLbdEM13382793;     ilmiDyNAmitfJikLbdEM13382793 = ilmiDyNAmitfJikLbdEM80054396;     ilmiDyNAmitfJikLbdEM80054396 = ilmiDyNAmitfJikLbdEM28809257;     ilmiDyNAmitfJikLbdEM28809257 = ilmiDyNAmitfJikLbdEM74853357;     ilmiDyNAmitfJikLbdEM74853357 = ilmiDyNAmitfJikLbdEM65680729;     ilmiDyNAmitfJikLbdEM65680729 = ilmiDyNAmitfJikLbdEM52658;     ilmiDyNAmitfJikLbdEM52658 = ilmiDyNAmitfJikLbdEM18502763;     ilmiDyNAmitfJikLbdEM18502763 = ilmiDyNAmitfJikLbdEM28277266;     ilmiDyNAmitfJikLbdEM28277266 = ilmiDyNAmitfJikLbdEM19278697;     ilmiDyNAmitfJikLbdEM19278697 = ilmiDyNAmitfJikLbdEM40092681;     ilmiDyNAmitfJikLbdEM40092681 = ilmiDyNAmitfJikLbdEM61670621;     ilmiDyNAmitfJikLbdEM61670621 = ilmiDyNAmitfJikLbdEM97472646;     ilmiDyNAmitfJikLbdEM97472646 = ilmiDyNAmitfJikLbdEM26207565;     ilmiDyNAmitfJikLbdEM26207565 = ilmiDyNAmitfJikLbdEM23719457;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ZvjsZmmWTOpDrlhCHZze7181515() {     float tKSDKCxDpbKMEieVQYbB94371123 = -504211686;    float tKSDKCxDpbKMEieVQYbB17915306 = -779679028;    float tKSDKCxDpbKMEieVQYbB36085177 = -285866361;    float tKSDKCxDpbKMEieVQYbB47386320 = -709313645;    float tKSDKCxDpbKMEieVQYbB73868212 = -834513949;    float tKSDKCxDpbKMEieVQYbB46079205 = 12531335;    float tKSDKCxDpbKMEieVQYbB79759278 = -419032570;    float tKSDKCxDpbKMEieVQYbB32777590 = -96413088;    float tKSDKCxDpbKMEieVQYbB34032634 = -241599278;    float tKSDKCxDpbKMEieVQYbB87282486 = -20657618;    float tKSDKCxDpbKMEieVQYbB91120882 = -495845299;    float tKSDKCxDpbKMEieVQYbB75735622 = -929793862;    float tKSDKCxDpbKMEieVQYbB52587971 = -834178478;    float tKSDKCxDpbKMEieVQYbB44660506 = -666159556;    float tKSDKCxDpbKMEieVQYbB41528218 = -736849555;    float tKSDKCxDpbKMEieVQYbB54281756 = -319329501;    float tKSDKCxDpbKMEieVQYbB68338259 = -761710886;    float tKSDKCxDpbKMEieVQYbB60020997 = -498453923;    float tKSDKCxDpbKMEieVQYbB97605706 = -827203018;    float tKSDKCxDpbKMEieVQYbB44869520 = -850639919;    float tKSDKCxDpbKMEieVQYbB18980285 = -480816047;    float tKSDKCxDpbKMEieVQYbB19557441 = -858939394;    float tKSDKCxDpbKMEieVQYbB30800894 = -206801911;    float tKSDKCxDpbKMEieVQYbB56615952 = -178391965;    float tKSDKCxDpbKMEieVQYbB40516697 = -800020085;    float tKSDKCxDpbKMEieVQYbB44111227 = -470824481;    float tKSDKCxDpbKMEieVQYbB10695270 = -918686466;    float tKSDKCxDpbKMEieVQYbB85409781 = -803128705;    float tKSDKCxDpbKMEieVQYbB56843189 = -131675442;    float tKSDKCxDpbKMEieVQYbB47401254 = -856896559;    float tKSDKCxDpbKMEieVQYbB25300110 = -391762434;    float tKSDKCxDpbKMEieVQYbB23323395 = -724595036;    float tKSDKCxDpbKMEieVQYbB56811082 = 19101075;    float tKSDKCxDpbKMEieVQYbB34340142 = -560323207;    float tKSDKCxDpbKMEieVQYbB72391275 = -654062905;    float tKSDKCxDpbKMEieVQYbB55231319 = -56144574;    float tKSDKCxDpbKMEieVQYbB42822058 = -905580854;    float tKSDKCxDpbKMEieVQYbB83907871 = -491131010;    float tKSDKCxDpbKMEieVQYbB35940297 = -831753710;    float tKSDKCxDpbKMEieVQYbB77025522 = -132383143;    float tKSDKCxDpbKMEieVQYbB63104806 = 51478631;    float tKSDKCxDpbKMEieVQYbB19810182 = -576998501;    float tKSDKCxDpbKMEieVQYbB90576002 = -591291388;    float tKSDKCxDpbKMEieVQYbB50068859 = -371814349;    float tKSDKCxDpbKMEieVQYbB92221411 = -430801197;    float tKSDKCxDpbKMEieVQYbB63996205 = -320920157;    float tKSDKCxDpbKMEieVQYbB38277009 = -468940362;    float tKSDKCxDpbKMEieVQYbB37190471 = -486419714;    float tKSDKCxDpbKMEieVQYbB12722933 = -95134029;    float tKSDKCxDpbKMEieVQYbB26503885 = -122613039;    float tKSDKCxDpbKMEieVQYbB76478691 = -481980707;    float tKSDKCxDpbKMEieVQYbB76895922 = 81087497;    float tKSDKCxDpbKMEieVQYbB2724449 = -29406130;    float tKSDKCxDpbKMEieVQYbB50710234 = -180408774;    float tKSDKCxDpbKMEieVQYbB88201229 = -150520799;    float tKSDKCxDpbKMEieVQYbB74813683 = -645272292;    float tKSDKCxDpbKMEieVQYbB87114411 = -472877118;    float tKSDKCxDpbKMEieVQYbB79469224 = -7474397;    float tKSDKCxDpbKMEieVQYbB6869624 = -909293561;    float tKSDKCxDpbKMEieVQYbB29756986 = -263689469;    float tKSDKCxDpbKMEieVQYbB35383935 = -68782200;    float tKSDKCxDpbKMEieVQYbB94349496 = -615903865;    float tKSDKCxDpbKMEieVQYbB75934401 = -964737647;    float tKSDKCxDpbKMEieVQYbB86631379 = -384702720;    float tKSDKCxDpbKMEieVQYbB61982377 = -628895185;    float tKSDKCxDpbKMEieVQYbB67797488 = -771250263;    float tKSDKCxDpbKMEieVQYbB18924540 = -848894938;    float tKSDKCxDpbKMEieVQYbB18247830 = -173855272;    float tKSDKCxDpbKMEieVQYbB72269231 = 87903349;    float tKSDKCxDpbKMEieVQYbB86296899 = -580704981;    float tKSDKCxDpbKMEieVQYbB11459699 = -413748647;    float tKSDKCxDpbKMEieVQYbB84430387 = -170579877;    float tKSDKCxDpbKMEieVQYbB24080701 = -666700214;    float tKSDKCxDpbKMEieVQYbB20580184 = -594819876;    float tKSDKCxDpbKMEieVQYbB81764713 = -802118550;    float tKSDKCxDpbKMEieVQYbB99170103 = -903817546;    float tKSDKCxDpbKMEieVQYbB28981438 = -167648006;    float tKSDKCxDpbKMEieVQYbB80732035 = -834987563;    float tKSDKCxDpbKMEieVQYbB64394541 = -747590768;    float tKSDKCxDpbKMEieVQYbB76520492 = -379099928;    float tKSDKCxDpbKMEieVQYbB5834219 = 98115881;    float tKSDKCxDpbKMEieVQYbB73504798 = -332266752;    float tKSDKCxDpbKMEieVQYbB72686849 = -607994676;    float tKSDKCxDpbKMEieVQYbB30339304 = 90937597;    float tKSDKCxDpbKMEieVQYbB70922563 = -274915852;    float tKSDKCxDpbKMEieVQYbB48404187 = -372849931;    float tKSDKCxDpbKMEieVQYbB20598947 = -595188907;    float tKSDKCxDpbKMEieVQYbB6100849 = -800490152;    float tKSDKCxDpbKMEieVQYbB46138912 = -309802408;    float tKSDKCxDpbKMEieVQYbB97577591 = 91209387;    float tKSDKCxDpbKMEieVQYbB68116907 = -583267457;    float tKSDKCxDpbKMEieVQYbB63352833 = -798106458;    float tKSDKCxDpbKMEieVQYbB77038248 = -581837450;    float tKSDKCxDpbKMEieVQYbB6183311 = -468064242;    float tKSDKCxDpbKMEieVQYbB41641587 = 36399056;    float tKSDKCxDpbKMEieVQYbB68755310 = -332617505;    float tKSDKCxDpbKMEieVQYbB43875780 = -612260855;    float tKSDKCxDpbKMEieVQYbB3944623 = -106588669;    float tKSDKCxDpbKMEieVQYbB88086482 = -742919164;    float tKSDKCxDpbKMEieVQYbB24423923 = -504211686;     tKSDKCxDpbKMEieVQYbB94371123 = tKSDKCxDpbKMEieVQYbB17915306;     tKSDKCxDpbKMEieVQYbB17915306 = tKSDKCxDpbKMEieVQYbB36085177;     tKSDKCxDpbKMEieVQYbB36085177 = tKSDKCxDpbKMEieVQYbB47386320;     tKSDKCxDpbKMEieVQYbB47386320 = tKSDKCxDpbKMEieVQYbB73868212;     tKSDKCxDpbKMEieVQYbB73868212 = tKSDKCxDpbKMEieVQYbB46079205;     tKSDKCxDpbKMEieVQYbB46079205 = tKSDKCxDpbKMEieVQYbB79759278;     tKSDKCxDpbKMEieVQYbB79759278 = tKSDKCxDpbKMEieVQYbB32777590;     tKSDKCxDpbKMEieVQYbB32777590 = tKSDKCxDpbKMEieVQYbB34032634;     tKSDKCxDpbKMEieVQYbB34032634 = tKSDKCxDpbKMEieVQYbB87282486;     tKSDKCxDpbKMEieVQYbB87282486 = tKSDKCxDpbKMEieVQYbB91120882;     tKSDKCxDpbKMEieVQYbB91120882 = tKSDKCxDpbKMEieVQYbB75735622;     tKSDKCxDpbKMEieVQYbB75735622 = tKSDKCxDpbKMEieVQYbB52587971;     tKSDKCxDpbKMEieVQYbB52587971 = tKSDKCxDpbKMEieVQYbB44660506;     tKSDKCxDpbKMEieVQYbB44660506 = tKSDKCxDpbKMEieVQYbB41528218;     tKSDKCxDpbKMEieVQYbB41528218 = tKSDKCxDpbKMEieVQYbB54281756;     tKSDKCxDpbKMEieVQYbB54281756 = tKSDKCxDpbKMEieVQYbB68338259;     tKSDKCxDpbKMEieVQYbB68338259 = tKSDKCxDpbKMEieVQYbB60020997;     tKSDKCxDpbKMEieVQYbB60020997 = tKSDKCxDpbKMEieVQYbB97605706;     tKSDKCxDpbKMEieVQYbB97605706 = tKSDKCxDpbKMEieVQYbB44869520;     tKSDKCxDpbKMEieVQYbB44869520 = tKSDKCxDpbKMEieVQYbB18980285;     tKSDKCxDpbKMEieVQYbB18980285 = tKSDKCxDpbKMEieVQYbB19557441;     tKSDKCxDpbKMEieVQYbB19557441 = tKSDKCxDpbKMEieVQYbB30800894;     tKSDKCxDpbKMEieVQYbB30800894 = tKSDKCxDpbKMEieVQYbB56615952;     tKSDKCxDpbKMEieVQYbB56615952 = tKSDKCxDpbKMEieVQYbB40516697;     tKSDKCxDpbKMEieVQYbB40516697 = tKSDKCxDpbKMEieVQYbB44111227;     tKSDKCxDpbKMEieVQYbB44111227 = tKSDKCxDpbKMEieVQYbB10695270;     tKSDKCxDpbKMEieVQYbB10695270 = tKSDKCxDpbKMEieVQYbB85409781;     tKSDKCxDpbKMEieVQYbB85409781 = tKSDKCxDpbKMEieVQYbB56843189;     tKSDKCxDpbKMEieVQYbB56843189 = tKSDKCxDpbKMEieVQYbB47401254;     tKSDKCxDpbKMEieVQYbB47401254 = tKSDKCxDpbKMEieVQYbB25300110;     tKSDKCxDpbKMEieVQYbB25300110 = tKSDKCxDpbKMEieVQYbB23323395;     tKSDKCxDpbKMEieVQYbB23323395 = tKSDKCxDpbKMEieVQYbB56811082;     tKSDKCxDpbKMEieVQYbB56811082 = tKSDKCxDpbKMEieVQYbB34340142;     tKSDKCxDpbKMEieVQYbB34340142 = tKSDKCxDpbKMEieVQYbB72391275;     tKSDKCxDpbKMEieVQYbB72391275 = tKSDKCxDpbKMEieVQYbB55231319;     tKSDKCxDpbKMEieVQYbB55231319 = tKSDKCxDpbKMEieVQYbB42822058;     tKSDKCxDpbKMEieVQYbB42822058 = tKSDKCxDpbKMEieVQYbB83907871;     tKSDKCxDpbKMEieVQYbB83907871 = tKSDKCxDpbKMEieVQYbB35940297;     tKSDKCxDpbKMEieVQYbB35940297 = tKSDKCxDpbKMEieVQYbB77025522;     tKSDKCxDpbKMEieVQYbB77025522 = tKSDKCxDpbKMEieVQYbB63104806;     tKSDKCxDpbKMEieVQYbB63104806 = tKSDKCxDpbKMEieVQYbB19810182;     tKSDKCxDpbKMEieVQYbB19810182 = tKSDKCxDpbKMEieVQYbB90576002;     tKSDKCxDpbKMEieVQYbB90576002 = tKSDKCxDpbKMEieVQYbB50068859;     tKSDKCxDpbKMEieVQYbB50068859 = tKSDKCxDpbKMEieVQYbB92221411;     tKSDKCxDpbKMEieVQYbB92221411 = tKSDKCxDpbKMEieVQYbB63996205;     tKSDKCxDpbKMEieVQYbB63996205 = tKSDKCxDpbKMEieVQYbB38277009;     tKSDKCxDpbKMEieVQYbB38277009 = tKSDKCxDpbKMEieVQYbB37190471;     tKSDKCxDpbKMEieVQYbB37190471 = tKSDKCxDpbKMEieVQYbB12722933;     tKSDKCxDpbKMEieVQYbB12722933 = tKSDKCxDpbKMEieVQYbB26503885;     tKSDKCxDpbKMEieVQYbB26503885 = tKSDKCxDpbKMEieVQYbB76478691;     tKSDKCxDpbKMEieVQYbB76478691 = tKSDKCxDpbKMEieVQYbB76895922;     tKSDKCxDpbKMEieVQYbB76895922 = tKSDKCxDpbKMEieVQYbB2724449;     tKSDKCxDpbKMEieVQYbB2724449 = tKSDKCxDpbKMEieVQYbB50710234;     tKSDKCxDpbKMEieVQYbB50710234 = tKSDKCxDpbKMEieVQYbB88201229;     tKSDKCxDpbKMEieVQYbB88201229 = tKSDKCxDpbKMEieVQYbB74813683;     tKSDKCxDpbKMEieVQYbB74813683 = tKSDKCxDpbKMEieVQYbB87114411;     tKSDKCxDpbKMEieVQYbB87114411 = tKSDKCxDpbKMEieVQYbB79469224;     tKSDKCxDpbKMEieVQYbB79469224 = tKSDKCxDpbKMEieVQYbB6869624;     tKSDKCxDpbKMEieVQYbB6869624 = tKSDKCxDpbKMEieVQYbB29756986;     tKSDKCxDpbKMEieVQYbB29756986 = tKSDKCxDpbKMEieVQYbB35383935;     tKSDKCxDpbKMEieVQYbB35383935 = tKSDKCxDpbKMEieVQYbB94349496;     tKSDKCxDpbKMEieVQYbB94349496 = tKSDKCxDpbKMEieVQYbB75934401;     tKSDKCxDpbKMEieVQYbB75934401 = tKSDKCxDpbKMEieVQYbB86631379;     tKSDKCxDpbKMEieVQYbB86631379 = tKSDKCxDpbKMEieVQYbB61982377;     tKSDKCxDpbKMEieVQYbB61982377 = tKSDKCxDpbKMEieVQYbB67797488;     tKSDKCxDpbKMEieVQYbB67797488 = tKSDKCxDpbKMEieVQYbB18924540;     tKSDKCxDpbKMEieVQYbB18924540 = tKSDKCxDpbKMEieVQYbB18247830;     tKSDKCxDpbKMEieVQYbB18247830 = tKSDKCxDpbKMEieVQYbB72269231;     tKSDKCxDpbKMEieVQYbB72269231 = tKSDKCxDpbKMEieVQYbB86296899;     tKSDKCxDpbKMEieVQYbB86296899 = tKSDKCxDpbKMEieVQYbB11459699;     tKSDKCxDpbKMEieVQYbB11459699 = tKSDKCxDpbKMEieVQYbB84430387;     tKSDKCxDpbKMEieVQYbB84430387 = tKSDKCxDpbKMEieVQYbB24080701;     tKSDKCxDpbKMEieVQYbB24080701 = tKSDKCxDpbKMEieVQYbB20580184;     tKSDKCxDpbKMEieVQYbB20580184 = tKSDKCxDpbKMEieVQYbB81764713;     tKSDKCxDpbKMEieVQYbB81764713 = tKSDKCxDpbKMEieVQYbB99170103;     tKSDKCxDpbKMEieVQYbB99170103 = tKSDKCxDpbKMEieVQYbB28981438;     tKSDKCxDpbKMEieVQYbB28981438 = tKSDKCxDpbKMEieVQYbB80732035;     tKSDKCxDpbKMEieVQYbB80732035 = tKSDKCxDpbKMEieVQYbB64394541;     tKSDKCxDpbKMEieVQYbB64394541 = tKSDKCxDpbKMEieVQYbB76520492;     tKSDKCxDpbKMEieVQYbB76520492 = tKSDKCxDpbKMEieVQYbB5834219;     tKSDKCxDpbKMEieVQYbB5834219 = tKSDKCxDpbKMEieVQYbB73504798;     tKSDKCxDpbKMEieVQYbB73504798 = tKSDKCxDpbKMEieVQYbB72686849;     tKSDKCxDpbKMEieVQYbB72686849 = tKSDKCxDpbKMEieVQYbB30339304;     tKSDKCxDpbKMEieVQYbB30339304 = tKSDKCxDpbKMEieVQYbB70922563;     tKSDKCxDpbKMEieVQYbB70922563 = tKSDKCxDpbKMEieVQYbB48404187;     tKSDKCxDpbKMEieVQYbB48404187 = tKSDKCxDpbKMEieVQYbB20598947;     tKSDKCxDpbKMEieVQYbB20598947 = tKSDKCxDpbKMEieVQYbB6100849;     tKSDKCxDpbKMEieVQYbB6100849 = tKSDKCxDpbKMEieVQYbB46138912;     tKSDKCxDpbKMEieVQYbB46138912 = tKSDKCxDpbKMEieVQYbB97577591;     tKSDKCxDpbKMEieVQYbB97577591 = tKSDKCxDpbKMEieVQYbB68116907;     tKSDKCxDpbKMEieVQYbB68116907 = tKSDKCxDpbKMEieVQYbB63352833;     tKSDKCxDpbKMEieVQYbB63352833 = tKSDKCxDpbKMEieVQYbB77038248;     tKSDKCxDpbKMEieVQYbB77038248 = tKSDKCxDpbKMEieVQYbB6183311;     tKSDKCxDpbKMEieVQYbB6183311 = tKSDKCxDpbKMEieVQYbB41641587;     tKSDKCxDpbKMEieVQYbB41641587 = tKSDKCxDpbKMEieVQYbB68755310;     tKSDKCxDpbKMEieVQYbB68755310 = tKSDKCxDpbKMEieVQYbB43875780;     tKSDKCxDpbKMEieVQYbB43875780 = tKSDKCxDpbKMEieVQYbB3944623;     tKSDKCxDpbKMEieVQYbB3944623 = tKSDKCxDpbKMEieVQYbB88086482;     tKSDKCxDpbKMEieVQYbB88086482 = tKSDKCxDpbKMEieVQYbB24423923;     tKSDKCxDpbKMEieVQYbB24423923 = tKSDKCxDpbKMEieVQYbB94371123;}
// Junk Finished
