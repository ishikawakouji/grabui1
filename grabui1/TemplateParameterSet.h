#pragma once
template <class T, typename U>
bool SetValue(GENAPI_NAMESPACE::INodeMap& nodemap, const char* nodename, U value) {
	bool bret = false;
	T cp(nodemap, nodename);
	if (cp.IsValid()) {
		std::cout << nodename << " = " << value << std::endl;
		cp.SetValue(value);
		bret = true;
	}
	else
	{
		std::cout << nodename << " is not valid" << std::endl;
	}
	return bret;
}

template <class T, typename U>
bool TrySetValue(GENAPI_NAMESPACE::INodeMap& nodemap, const char* nodename, U value) {
	T cp(nodemap, nodename);
	bool bret = false;
	if (cp.IsValid()) {
		bret = cp.TrySetValue(value);
		if (bret) {
			std::cout << nodename << " = " << value << std::endl;
		}
		else {
			std::cout << nodename << " failed " << value << std::endl;
		}
	}
	else
	{
		std::cout << nodename << " is not valid" << std::endl;
	}
	return bret;
}

template <class T, typename U, class V, typename W>
bool SelectAndSetValue(GENAPI_NAMESPACE::INodeMap& nodemap, const char* selectname, U selectvalue, const char* setname, W setvalue)
{
	bool bret = SetValue<T, U>(nodemap, selectname, selectvalue);
	if (bret) {
		bret = SetValue<V, W>(nodemap, setname, setvalue);
	}
	return bret;
}