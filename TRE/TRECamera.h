#ifndef __TRECAMERA_H__
#define __TRECAMERA_H__

#include <TRE/TREFacing.h>

class TRECamera
{
	public:
		TRECamera(void);
		TRECamera(const TRECamera& other);
		~TRECamera(void);

		TCVector getPosition(void) {return position;}
		void setPosition(const TCVector &pos) {position = pos;}
		TREFacing getFacing(void) {return facing;}
		void setFacing(const TREFacing &newFacing) { facing = newFacing; }
		char* getName(void) {return name;}
		void setName(char* n);
		TRECamera& operator=(const TRECamera& right) {return duplicate(right);}
		void project(const TCVector &eyeOffset);
		void rotate(const TCVector &rotation);
		void move(const TCVector &distance);
	protected:
		TRECamera& duplicate(const TRECamera& copyFrom);

		TCVector position;
		TREFacing facing;
		char* name;
};

#endif // __TRECAMERA_H__
