#pragma once
#include "Types.h"
#include "Container.h"
#include "rapidxml.hpp"
using namespace rapidxml;

using XmlNodeType = xml_node<WCHAR>;
using XmlDocumentType = xml_document<WCHAR>;
using XmlAttributeType = xml_attribute<WCHAR>;

class XmlNode
{
public:
	XmlNode(XmlNodeType* node = nullptr) : _node(node) { }

	auto IsValid() -> bool { return _node != nullptr; }

	auto GetBoolAttr(const WCHAR* key, bool defaultValue = false) -> bool;
	auto GetInt8Attr(const WCHAR* key, int8 defaultValue = 0) -> int8;
	auto GetInt16Attr(const WCHAR* key, int16 defaultValue = 0) -> int16;
	auto GetInt32Attr(const WCHAR* key, int32 defaultValue = 0) -> int32;
	auto GetInt64Attr(const WCHAR* key, int64 defaultValue = 0) -> int64;
	auto GetFloatAttr(const WCHAR* key, float defaultValue = 0.0f) -> float;
	auto GetDoubleAttr(const WCHAR* key, double defaultValue = 0.0) -> double;
	auto GetStringAttr(const WCHAR* key, const WCHAR* defaultValue = L"") -> const WCHAR*;

	auto GetBoolValue(bool defaultValue = false) -> bool;
	auto GetInt8Value(int8 defaultValue = 0) -> int8;
	auto GetInt16Value(int16 defaultValue = 0) -> int16;
	auto GetInt32Value(int32 defaultValue = 0) -> int32;
	auto GetInt64Value(int64 defaultValue = 0) -> int64;
	auto GetFloatValue(float defaultValue = 0.0f) -> float;
	auto GetDoubleValue(double defaultValue = 0.0) -> double;
	auto GetStringValue(const WCHAR* defaultValue = L"") -> const WCHAR*;

	auto FindChild(const WCHAR* key) -> XmlNode;
	auto FindChildren(const WCHAR* key) -> Vector<XmlNode>;

private:
	XmlNodeType* _node{ nullptr };
};

class XmlParser
{
public:
	auto ParseFromFile(const WCHAR* path, OUT XmlNode& root) -> bool;

private:
	std::shared_ptr<XmlDocumentType> _document{ nullptr };
	String _data;
};