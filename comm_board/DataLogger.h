/* 
 * File:   DataLogger.h
 * Author: Dan
 *
 * Created on December 17, 2014, 1:46 PM
 */

#ifndef DATALOGGER_H
#define	DATALOGGER_H


void initDataLogger(int frequency_s);

void dataLoggerStep(void);

void get_data_logs_handler();

void reset_data_logging_handler();

#endif	/* DATALOGGER_H */

