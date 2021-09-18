#pragma once

// PatchApplier provides the stuff for implementing patches for the specific game version
class PatchApplier {
public:
	virtual void ApplyPatches() = 0;
	void ApplyExPatches() {};
};