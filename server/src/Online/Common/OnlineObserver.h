#ifndef __ONLINE_OBSERVER_H__
#define __ONLINE_OBSERVER_H__

/**
* Abstract Class OnlineObserver. 
* Class XOnlineObserver is the observer of Online component.
* This class designed as an abstract class. So client developer has to re-implement all virtual functions.
*/
class OnlineComponent;
class OnlineObserver
{
public:
	/**
	* Virtual Function OnRequestSuccess.
	* The correct response.
	* <p>
	* @param onlineComponent.
	* @param func_id Function ID.
	*/
	virtual void OnRequestSuccess(OnlineComponent *onlineComponent, int func_id ) = 0;

	/**
	* Virtual Function OnRequestFailed.
	* The wrong response or the operation is failed on server side.
	* <p>
	* @param onlineComponent.
	* @param func_id Function ID.
	* @param error_code Error code.
	*/
	virtual void OnRequestFailed(OnlineComponent *onlineComponent, int func_id, int error_code ) = 0;
};

#endif