/* This file is derived from source code for the Nachos
   instructional operating system.  The Nachos copyright notice
   is reproduced in full below. */

/* Copyright (c) 1992-1996 The Regents of the University of California.
   All rights reserved.

   Permission to use, copy, modify, and distribute this software
   and its documentation for any purpose, without fee, and
   without written agreement is hereby granted, provided that the
   above copyright notice and the following two paragraphs appear
   in all copies of this software.

   IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO
   ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
   CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE
   AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA
   HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
   PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
   BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
   PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
   MODIFICATIONS.
*/

#include "threads/synch.h"
#include <stdio.h>
#include <string.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

/* Initializes semaphore SEMA to VALUE.  A semaphore is a
   nonnegative integer along with two atomic operators for
   manipulating it:

   - down or "P": wait for the value to become positive, then
     decrement it.

   - up or "V": increment the value (and wake up one waiting
     thread, if any). */
void
sema_init (struct semaphore *sema, unsigned value)
{
  ASSERT (sema != NULL);

  sema->value = value;
  list_init (&sema->waiters);
}


/* greater comparator to comapre priorities of threads to sort threads
   in the list in the correct positions. */

bool greater(struct list_elem *e1 , struct list_elem *e2 , void *aux){
  struct thread* t1 = list_entry(e1, struct thread ,elem);
  struct thread* t2 = list_entry(e2 ,struct thread ,elem);
  return t1->priority > t2->priority;
}


/* Down or "P" operation on a semaphore.  Waits for SEMA's value
   to become positive and then atomically decrements it.

   This function may sleep, so it must not be called within an
   interrupt handler.  This function may be called with
   interrupts disabled, but if it sleeps then the next scheduled
   thread will probably turn interrupts back on. */

void
sema_down (struct semaphore *sema)
{
  enum intr_level old_level;

  ASSERT (sema != NULL);
  ASSERT (!intr_context ());

  old_level = intr_disable ();
  // why while not if ????
  bool flag = false;
  while(sema->value == 0){
      list_insert_ordered (&sema->waiters, &thread_current()->elem, greater, NULL);
      /*
      if(!flag)
        list_insert_ordered (&sema->waiters, &thread_current()->elem, greater, NULL);
      flag = true;
      */
      thread_block ();
  }
  sema->value--;
  intr_set_level (old_level);
}

/* Down or "P" operation on a semaphore, but only if the
   semaphore is not already 0.  Returns true if the semaphore is
   decremented, false otherwise.

   This function may be called from an interrupt handler. */
bool
sema_try_down (struct semaphore *sema)
{
  enum intr_level old_level;
  bool success;

  ASSERT (sema != NULL);

  old_level = intr_disable ();
  if (sema->value > 0)
    {
      sema->value--;
      success = true;
    }
  else
    success = false;
  intr_set_level (old_level);

  return success;
}

/* Up or "V" operation on a semaphore.  Increments SEMA's value
   and wakes up one thread of those waiting for SEMA, if any.

   This function may be called from an interrupt handler. */
void
sema_up (struct semaphore *sema)
{
  enum intr_level old_level;

  ASSERT (sema != NULL);

  old_level = intr_disable ();
  sema->value++;
  if(!list_empty(&sema->waiters)){
    list_sort(&sema->waiters, greater, NULL);
    struct thread *t = list_entry (list_pop_front (&sema->waiters), struct thread, elem);
    thread_unblock(t);
  }
  intr_set_level (old_level);
}

static void sema_test_helper (void *sema_);

/* Self-test for semaphores that makes control "ping-pong"
   between a pair of threads.  Insert calls to printf() to see
   what's going on. */
void
sema_self_test (void)
{
  struct semaphore sema[2];
  int i;

  sema_init (&sema[0], 0);
  sema_init (&sema[1], 0);
  thread_create ("sema-test", PRI_DEFAULT, sema_test_helper, &sema);
  for (i = 0; i < 10; i++)
    {
      sema_up (&sema[0]);
      sema_down (&sema[1]);
    }
}

/* Thread function used by sema_self_test(). */
static void
sema_test_helper (void *sema_)
{
  struct semaphore *sema = sema_;
  int i;

  for (i = 0; i < 10; i++)
    {
      sema_down (&sema[0]);
      sema_up (&sema[1]);
    }
}

/* Initializes LOCK.  A lock can be held by at most a single
   thread at any given time.  Our locks are not "recursive", that
   is, it is an error for the thread currently holding a lock to
   try to acquire that lock.

   A lock is a specialization of a semaphore with an initial
   value of 1.  The difference between a lock and such a
   semaphore is twofold.  First, a semaphore can have a value
   greater than 1, but a lock can only be owned by a single
   thread at a time.  Second, a semaphore does not have an owner,
   meaning that one thread can "down" the semaphore and then
   another one "up" it, but with a lock the same thread must both
   acquire and release it.  When these restrictions prove
   onerous, it's a good sign that a semaphore should be used,
   instead of a lock. */
void
lock_init (struct lock *lock)
{
  ASSERT (lock != NULL);
  lock->holder = NULL;
  lock->priority = 0;
  sema_init (&lock->semaphore, 1);
}

// what is this function ??
/*
void sema_alt(struct semaphore *sema, struct lock *lock){
  enum intr_level old_level;

  ASSERT (sema != NULL);
  ASSERT (!intr_context ());

  old_level = intr_disable ();

  while(sema->value == 0 || lock->holder != NULL){
      list_insert_ordered (&sema->waiters, &thread_current()->elem,
                              less, NULL);
      //list_push_back (&sema->waiters, &thread_current ()->elem);
      thread_block ();
  }
  sema->value--;
  intr_set_level (old_level);
}*/

// what is this comparator ??
/*
bool comparator(struct list_elem *e1 ,struct list_elem *e2,void* aux){
  struct lock  *lock1 = list_entry(e1 , struct lock , lock_elem);
  struct lock  *lock2 = list_entry(e2 , struct lock , lock_elem);
  struct semaphore *sema1 = &lock1->semaphore;
  struct semaphore *sema2 = &lock2->semaphore;

  return list_entry(list_begin(&sema1->waiters),struct thread ,elem)->priority >
   list_entry(list_begin(&sema2->waiters),struct thread ,elem)->priority;
}*/



bool greater_comparator_sort_lock_priority(struct list_elem *e1, struct list_elem *e2, void* aux){
  struct lock *lock1 = list_entry(e1 , struct lock , lock_elem);
  struct lock *lock2 = list_entry(e2 , struct lock , lock_elem);
  struct semaphore *sema1 = &lock1->semaphore;
  struct semaphore *sema2 = &lock2->semaphore;
  return list_entry(list_begin(&sema1->waiters),struct thread ,elem)->priority >
         list_entry(list_begin(&sema2->waiters),struct thread ,elem)->priority;
}


/* Acquires LOCK, sleeping until it becomes available if
   necessary.  The lock must not already be held by the current
   thread.

   This function may sleep, so it must not be called within an
   interrupt handler.  This function may be called with
   interrupts disabled, but interrupts will be turned back on if
   we need to sleep. */

void
lock_acquire (struct lock *lock)
{
  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (!lock_held_by_current_thread (lock));
  struct lock * temp_lock = lock;
  while(temp_lock->holder != NULL && temp_lock->holder->priority < thread_current()->priority){
    thread_donate(temp_lock->holder,thread_current()->priority);
    temp_lock = temp_lock->holder->required_lock;
    if(temp_lock == NULL)break;
  }
  thread_current()->required_lock = lock;
  sema_down(&lock->semaphore);
  thread_current()->required_lock = NULL;
  lock->holder = thread_current ();
  lock->priority = thread_current()->priority;
  list_push_back(&thread_current()->locks,&lock->lock_elem);
  list_sort(&thread_current()->locks, greater_comparator_sort_lock_priority, NULL);
}



/* Tries to acquires LOCK and returns true if successful or false
   on failure.  The lock must not already be held by the current
   thread.

   This function will not sleep, so it may be called within an
   interrupt handler. */
bool
lock_try_acquire (struct lock *lock)
{
  bool success;

  ASSERT (lock != NULL);
  ASSERT (!lock_held_by_current_thread (lock));

  success = sema_try_down (&lock->semaphore);
  if (success)
    lock->holder = thread_current ();
  return success;
}

// what is this function ??
int max_donation(struct list* list){
  printf("1\n");
  ASSERT(list != NULL);
  ASSERT(!list_empty(&list));
  printf("1\n");
  int max_priority = 0;
  struct lock *lock;
  struct semaphore *sema;
  struct list_elem *e;
  printf("1\n");
  for(e = list_begin(&list);e != list_end(&list);e = list_next(e)){
    printf("1\n");
    lock = list_entry(e , struct lock , lock_elem);
    printf("2\n");
    sema = &lock->semaphore;
    printf("3\n");
    int tmp = list_entry(list_begin(&sema->waiters),struct thread ,elem)->priority;
    printf("4\n");
    if(tmp > max_priority){
      max_priority = tmp;
    }
    printf("5\n");
  }
  return max_priority;
}


/* Releases LOCK, which must be owned by the current thread.

   An interrupt handler cannot acquire a lock, so it does not
   make sense to try to release a lock within an interrupt
   handler. */

void
lock_release (struct lock *lock)
{
   ASSERT (lock != NULL);
   ASSERT (lock_held_by_current_thread (lock));
   struct thread* t = lock->holder;
   if(!list_empty(&t->locks)){
     list_remove(&lock->lock_elem);
   }
   lock->holder = NULL;

   sema_up (&lock->semaphore);
   //max in first acquired lock
   if(!list_empty(&thread_current()->locks)){
     struct list_elem *e;
     e = list_begin(&thread_current()->locks);
     struct lock *l;
     l = list_entry(e , struct lock , lock_elem);
     struct semaphore *sema = &l->semaphore;

     if(!list_empty(&sema->waiters)){
       int tmp = list_entry(list_begin(&sema->waiters),struct thread ,elem)->priority;
       thread_return_donation(t, tmp);

     } else{
       thread_return_donation(t, thread_current()->original_priority);
     }

   } else{
     thread_return_donation(t, thread_current()->original_priority);
   }
}

/* Returns true if the current thread holds LOCK, false
   otherwise.  (Note that testing whether some other thread holds
   a lock would be racy.) */
bool
lock_held_by_current_thread (const struct lock *lock)
{
  ASSERT (lock != NULL);
  return lock->holder == thread_current ();
}

/* One semaphore in a list. */
struct semaphore_elem
  {
    struct list_elem elem;              /* List element. */
    struct semaphore semaphore;         /* This semaphore. */
  };

/* Initializes condition variable COND.  A condition variable
   allows one piece of code to signal a condition and cooperating
   code to receive the signal and act upon it. */
void
cond_init (struct condition *cond)
{
  ASSERT (cond != NULL);
  list_init (&cond->waiters);
}

bool greater_condition_variable_priority_comparator(struct list_elem *e1, struct list_elem *e2, void* aux){

  struct semaphore *s1 = &list_entry(e1, struct semaphore_elem, elem)->semaphore;
  struct semaphore *s2 = &list_entry(e2, struct semaphore_elem, elem)->semaphore;

  return (list_entry(list_front(&s1->waiters), struct thread, elem)->priority >
        list_entry(list_front(&s2->waiters), struct thread, elem)->priority);

}

/* Atomically releases LOCK and waits for COND to be signaled by
   some other piece of code.  After COND is signaled, LOCK is
   reacquired before returning.  LOCK must be held before calling
   this function.

   The monitor implemented by this function is "Mesa" style, not
   "Hoare" style, that is, sending and receiving a signal are not
   an atomic operation.  Thus, typically the caller must recheck
   the condition after the wait completes and, if necessary, wait
   again.

   A given condition variable is associated with only a single
   lock, but one lock may be associated with any number of
   condition variables.  That is, there is a one-to-many mapping
   from locks to condition variables.

   This function may sleep, so it must not be called within an
   interrupt handler.  This function may be called with
   interrupts disabled, but interrupts will be turned back on if
   we need to sleep. */
void
cond_wait (struct condition *cond, struct lock *lock)
{
  struct semaphore_elem waiter;

  ASSERT (cond != NULL);
  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (lock_held_by_current_thread (lock));

  sema_init (&waiter.semaphore, 0);
  list_push_back (&cond->waiters, &waiter.elem);
  lock_release (lock);
  sema_down (&waiter.semaphore);
  lock_acquire (lock);
}

/* If any threads are waiting on COND (protected by LOCK), then
   this function signals one of them to wake up from its wait.
   LOCK must be held before calling this function.

   An interrupt handler cannot acquire a lock, so it does not
   make sense to try to signal a condition variable within an
   interrupt handler. */
void
cond_signal (struct condition *cond, struct lock *lock UNUSED)
{
  ASSERT (cond != NULL);
  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (lock_held_by_current_thread (lock));

  if (list_empty (&cond->waiters))return;

  /* sort the waiting threads and get the thread with the smalled priority. */
  list_sort(&cond->waiters, greater_condition_variable_priority_comparator, NULL);
  sema_up (&list_entry (list_pop_front (&cond->waiters),
                          struct semaphore_elem, elem)->semaphore);
}

/* Wakes up all threads, if any, waiting on COND (protected by
   LOCK).  LOCK must be held before calling this function.

   An interrupt handler cannot acquire a lock, so it does not
   make sense to try to signal a condition variable within an
   interrupt handler. */
void
cond_broadcast (struct condition *cond, struct lock *lock)
{
  ASSERT (cond != NULL);
  ASSERT (lock != NULL);

  while (!list_empty (&cond->waiters))
    cond_signal (cond, lock);
}