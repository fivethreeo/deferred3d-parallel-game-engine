#pragma once

#include <d3d/assetmanager/Asset.h>

#include <d3d/system/ComputeSystem.h>

#include <assert.h>

namespace d3d {
	class ComputeProgram : public Asset {
	private:
		cl::Program _program;

	public:
		// Inherited from Asset
		bool createAsset(const std::string &name, void* pData);

		// Unused, error when user attempts to use this function
		bool createAsset(const std::string &name) {
			assert(false);

			return false;
		}

		cl::Program &getProgram() {
			return _program;
		}

		// Asset factory
		static Asset* assetFactory() {
			return new ComputeProgram();
		}
	};
}