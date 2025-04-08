/*
 * File: pot_handler.h
 * Description: Header for potentiometer height tracking functions.
 */

#ifndef POT_HANDLER_H
#define POT_HANDLER_H

void initPot();
int getRawPotValue();
float getHeight();
void setZeroHeight();
void setMaxHeight();
void setMinHeight();

#endif // POT_HANDLER_H
