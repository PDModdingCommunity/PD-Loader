#include <iostream>

using namespace System::Resources;
using namespace System::Reflection;
using namespace System;
using namespace System::IO;

ref class i18n
{
public:
	static ResourceManager^ ResMgr = gcnew ResourceManager("Launcher.Resources", i18n::typeid->Assembly);

	static String^ GetStringFallback(String^ name) {
		String^ result = ResMgr->GetString(name);
		return result != nullptr ? result : name;
	}
};

