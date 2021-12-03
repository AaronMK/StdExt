#include <StdExt/Serialize/CodeGen/Namespace.h>

#include "NamespaceInternal.h"

using namespace std;
using namespace StdExt;

namespace StdExt::Serialize::CodeGen
{
	const StdExt::String & Namespace::name() const
	{
		return mNamespaceInternal->Name;
	}

	Namespace Namespace::getNamespace(const StdExt::String& pName)
	{
		auto itr = mNamespaceInternal->Namespaces.find(pName);

		shared_ptr<NamespaceInternal> childInternal;

		if (mNamespaceInternal->Namespaces.end() == itr)
		{
			childInternal = make_shared<NamespaceInternal>();

			childInternal->FullName = name() + "." + pName;
			childInternal->Name = childInternal->FullName.substr(name().length() + 1);

			childInternal->Parent = mNamespaceInternal;

			mNamespaceInternal->Namespaces[pName] = childInternal;
		}
		else
		{
			childInternal = itr->second;
		}

		Namespace nsReturn;
		nsReturn.mNamespaceInternal = childInternal;
		nsReturn.mDocInternal = mDocInternal;

		return nsReturn;
	}
}