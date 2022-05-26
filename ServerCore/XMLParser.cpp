#include "pch.h"
#include "XmlParser.h"
#include "FileUtils.h"
#include "CoreGlobal.h"

/*-------------
	XmlNode
--------------*/

_locale_t kr = _create_locale(LC_NUMERIC, "kor");


auto XmlNode::GetBoolAttr(const WCHAR* key, bool defaultValue) -> bool
{
	XmlAttributeType* attr = _node->first_attribute(key);
	if (attr)
		return ::_wcsicmp(attr->value(), L"true") == 0;

	return defaultValue;
}

auto XmlNode::GetInt8Attr(const WCHAR* key, int8 defaultValue) -> int8
{
	XmlAttributeType* attr = _node->first_attribute(key);
	if (attr)
		return static_cast<int8>(::_wtoi(attr->value()));

	return defaultValue;
}

auto XmlNode::GetInt16Attr(const WCHAR* key, int16 defaultValue) -> int16
{
	XmlAttributeType* attr = _node->first_attribute(key);
	if (attr)
		return static_cast<int16>(::_wtoi(attr->value()));

	return defaultValue;
}

auto XmlNode::GetInt32Attr(const WCHAR* key, int32 defaultValue) -> int32
{
	XmlAttributeType* attr = _node->first_attribute(key);
	if (attr)
		return ::_wtoi(attr->value());

	return defaultValue;
}

auto XmlNode::GetInt64Attr(const WCHAR* key, int64 defaultValue) -> int64
{
	xml_attribute<WCHAR>* attr = _node->first_attribute(key);
	if (attr)
		return ::_wtoi64(attr->value());

	return defaultValue;
}

auto XmlNode::GetFloatAttr(const WCHAR* key, float defaultValue) -> float
{
	XmlAttributeType* attr = _node->first_attribute(key);
	if (attr)
		return static_cast<float>(::_wtof(attr->value()));

	return defaultValue;
}

auto XmlNode::GetDoubleAttr(const WCHAR* key, double defaultValue) -> double
{
	XmlAttributeType* attr = _node->first_attribute(key);
	if (attr)
		return ::_wtof_l(attr->value(), kr);

	return defaultValue;
}

auto XmlNode::GetStringAttr(const WCHAR* key, const WCHAR* defaultValue) -> const WCHAR*
{
	XmlAttributeType* attr = _node->first_attribute(key);
	if (attr)
		return attr->value();

	return defaultValue;
}

auto XmlNode::GetBoolValue(bool defaultValue) -> bool
{
	WCHAR* val = _node->value();
	if (val)
		return ::_wcsicmp(val, L"true") == 0;

	return defaultValue;
}

auto XmlNode::GetInt8Value(int8 defaultValue) -> int8
{
	WCHAR* val = _node->value();
	if (val)
		return static_cast<int8>(::_wtoi(val));

	return defaultValue;
}

auto XmlNode::GetInt16Value(int16 defaultValue) -> int16
{
	WCHAR* val = _node->value();
	if (val)
		return static_cast<int16>(::_wtoi(val));
	return defaultValue;
}

auto XmlNode::GetInt32Value(int32 defaultValue) -> int32
{
	WCHAR* val = _node->value();
	if (val)
		return static_cast<int32>(::_wtoi(val));

	return defaultValue;
}

auto XmlNode::GetInt64Value(int64 defaultValue) -> int64
{
	WCHAR* val = _node->value();
	if (val)
		return static_cast<int64>(::_wtoi64(val));

	return defaultValue;
}

auto XmlNode::GetFloatValue(float defaultValue) -> float
{
	WCHAR* val = _node->value();
	if (val)
		return static_cast<float>(::_wtof(val));

	return defaultValue;
}

auto XmlNode::GetDoubleValue(double defaultValue) -> double
{
	WCHAR* val = _node->value();
	if (val)
		return ::_wtof_l(val, kr);

	return defaultValue;
}

auto XmlNode::GetStringValue(const WCHAR* defaultValue) -> const WCHAR*
{
	WCHAR* val = _node->first_node()->value();
	if (val)
		return val;

	return defaultValue;
}

auto XmlNode::FindChild(const WCHAR* key) -> XmlNode
{
	return XmlNode(_node->first_node(key));
}

auto XmlNode::FindChildren(const WCHAR* key) -> Vector<XmlNode>
{
	Vector<XmlNode> nodes;

	xml_node<WCHAR>* node = _node->first_node(key);
	while (node)
	{
		nodes.push_back(XmlNode(node));
		node = node->next_sibling(key);
	}

	return nodes;
}

auto XmlParser::ParseFromFile(const WCHAR* path, OUT XmlNode& root) -> bool
{
	Vector<BYTE> bytes = FileUtils::ReadFile(path);
	_data = FileUtils::Convert(std::string(bytes.begin(), bytes.end()));

	if (_data.empty())
		return false;

	_document = MakeShared<XmlDocumentType>();
	_document->parse<0>(reinterpret_cast<WCHAR*>(&_data[0]));
	root = XmlNode(_document->first_node());
	return true;
}
