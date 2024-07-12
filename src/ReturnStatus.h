#ifndef _RETURN_STATUS_H_
#define _RETURN_STATUS_H_

struct ReturnStatus {
	static constexpr int OK{ 0 };
	static constexpr int INSTANCE_EXISTS{ 10 };
	static constexpr int ERROR_CREATING_WINDOW{ 11 };
	static constexpr int ERROR_REGISTERING_HOOK{ 12 };
};

#endif // !_RETURN_STATUS_H_

