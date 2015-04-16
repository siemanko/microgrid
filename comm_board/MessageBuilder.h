/* 
 * File:   MessageBuilder.h
 * Author: sidor
 *
 * Created on January 22, 2015, 1:24 AM
 */

#ifndef MESSAGEBUILDER_H
#define	MESSAGEBUILDER_H

typedef struct StructMessageBuilder {
    int next_char;
    int capacity;
    char ignore_me;
    char* message;
} MessageBuilder;

// Creates new message wrapper and allocates buffer memory.
void make_mb(MessageBuilder* mb, int capacity);

// Makes a copy of mb. If deep is 1 it copies internal buffer as well.
MessageBuilder* mb_copy(MessageBuilder* mb, int deep);

// Takes strlen(str) bytes.
void mb_add_string(MessageBuilder* mb, const char* str);

// Uses 1 byten
void mb_add_char(MessageBuilder* mb, char chr);

// Uses four bytes.
void mb_add_int(MessageBuilder* mb, int d);

// Uses six bytes.
void mb_add_float(MessageBuilder* mb, float f);

// Adds message to outgoing queue.
// Takes ownership of internal buffer, but not mb.
void mb_send(MessageBuilder* mb);

// Sends message out immediately. It is only safe to use if not from inside
// and interrupinterrupt.
// Takes ownership of internal buffer, but not mb.
void mb_send_unsafe(MessageBuilder* mb);


void mb_add_int_noprefix(MessageBuilder* mb, int d);

void mb_add_float_noprefix(MessageBuilder* mb, float d);

#endif	/* MESSAGEBUILDER_H */

