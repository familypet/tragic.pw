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
                                          

















































































































































































































































































