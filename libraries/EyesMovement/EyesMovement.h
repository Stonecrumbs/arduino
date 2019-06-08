/*
  EyesMovement.h - Test library for Wiring - description
  Copyright (c) 2006 John Doe.  All right reserved.
*/
// include types & constants of Wiring core API
#include <Arduino.h>

// ensure this library description is only included once
#ifndef EyesMovement_h
#define EyesMovement_h


// library interface description
class EyesMovement
{
  // user-accessible "public" interface
  public:
    EyesMovement(int);
    void drawEye(void);

  // library-accessible "private" interface
  private:
    void EyesMovement::initEye(void);
    void EyesMovement::paintPupiles(int pX);
    void EyesMovement::clearPupiles(int pX);
    void EyesMovement::clearRows(int row);
};

#endif

