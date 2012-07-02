/* GTS - Library for the manipulation of triangulated surfaces
 * Copyright (C) 2010 Luis Saavedra
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION: exception
 * @short_description: Simple try/catch/throw exception-handling
 * interface
 * @title: Exception Handling
 * @section_id:
 * @see_also:
 * @stability: Inestable
 * @include:
 * @Image:
 *
 */

#include "gts.h"

struct exception_context the_exception_context[1];

static void exception_destroy (GtsObject * object)
{
  GtsException * e = GTS_EXCEPTION (object);

  g_free (e->message); e->message = NULL;

  (* GTS_OBJECT_CLASS (gts_exception_class ())->parent_class->destroy) (object);
}

static void exception_class_init (GtsExceptionClass * klass)
{
  GTS_OBJECT_CLASS (klass)->destroy = exception_destroy;
}

static void exception_init (GtsException * e)
{
  e->message = NULL;
}

/**
 * gts_exception_class:
 *
 * Returns: the #GtsExceptionClass.
 */
GtsExceptionClass * gts_exception_class (void)
{
  static GtsExceptionClass * klass = NULL;

  if (klass == NULL) {
    GtsObjectClassInfo exception_info = {
      "GtsException",
      sizeof (GtsException),
      sizeof (GtsExceptionClass),
      (GtsObjectClassInitFunc) exception_class_init,
      (GtsObjectInitFunc) exception_init,
      (GtsArgSetFunc) NULL,
      (GtsArgGetFunc) NULL
    };
    klass = gts_object_class_new (gts_object_class (),
                                  &exception_info);
  }

  return klass;
}

/**
 * gts_exception_new_full:
 * @klass: a #GtsExceptionClass.
 * @error: a #GtsExceptionError.
 * @format: a standard printf() format string for the
 * explanation of the error.
 * @...: the arguments to insert in the explanation of
 * the error.
 *
 * Returns: a new #GtsException.
 */
GtsException * gts_exception_new_full (GtsExceptionClass * klass,
                                       GtsExceptionError error,
                                       gchar const * format, ...)
{
  GtsException * e;

  e = GTS_EXCEPTION (gts_object_new (GTS_OBJECT_CLASS (klass)));
  e->error = error;

  va_list argp;

  va_start (argp, format);
  gint length = g_vsnprintf (NULL, 0, format, argp);
  va_end (argp);

  e->message = g_malloc_n (length+1, sizeof(gchar));

  va_start (argp, format);
  g_vsnprintf (e->message, length+1, format, argp);
  va_end (argp);

  return e;
}

/**
 * gts_exception_set_message:
 * @e: a #GtsException.
 * @format: a standard printf() format string for the
 * explanation of the error.
 * @...: the arguments to set in the explanation of
 * the error.
 *
 * Returns: the new number of bytes in the explanation of error.
 */
gint gts_exception_set_message (GtsException * e, gchar const * format, ...)
{
  va_list argp;

  va_start (argp, format);
  gint length = g_vsnprintf (NULL, 0, format, argp);
  va_end (argp);

  gchar * new_message = g_malloc_n (length+1, sizeof(gchar));

  va_start (argp, format);
  g_vsnprintf (new_message, length+1, format, argp);
  va_end (argp);

  g_free (e->message);
  e->message = new_message;

  return length;
}
