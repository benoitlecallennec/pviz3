/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifndef SERVICE_EXCEPTION_H
#define SERVICE_EXCEPTION_H
#ifdef WIN32
#include "nbcppdll.h" 
#endif
#include <iostream>
#include <string>

using namespace std;
#ifdef WIN32
class NBCPPDLL_API ServiceException{
#else
class ServiceException{
#endif
public:
~ServiceException();
 ServiceException(const string msg);
   const string  getMessage();

private:
  const string  data;
};

#endif /*SERVICE_EXCEPTION_H*/
