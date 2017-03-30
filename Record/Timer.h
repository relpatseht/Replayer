/************************************************************************************\
 * Record - An Andrew Shurney Production                                         *
\************************************************************************************/

/*! \file		Timer.h
 *  \author		Andrew Shurney
 *  \brief		A timer whose calls don't get hooked
 */

#ifndef TIMER_H
#define TIMER_H

class Timer
{
	private:
		double frequency;
		unsigned startTime;

		unsigned GetTime() const;

	public:
		Timer();

		void Start();

		unsigned Elapsed();
};

#endif
