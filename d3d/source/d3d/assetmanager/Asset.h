#pragma once

#include <string>

#include <d3d/system/Uncopyable.h>

namespace d3d {
	class Asset : public Uncopyable {
	public:
		virtual ~Asset() {}
		virtual bool createAsset(const std::string &name) { return true; }
		virtual bool createAsset(const std::string &name, void* pData) { return true; }
	};
}
