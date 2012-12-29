/* GTS - Library for the manipulation of triangulated surfaces
 * Copyright (C) 1999 Stéphane Popinet
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/* MyObject: Header */

typedef struct _MyObject         MyObject;
typedef struct _MyObjectClass    MyObjectClass;

struct _MyObject {
  Parent parent;
  /* add extra data here */
};

struct _MyObjectClass {
  ParentClass parent_class;
  /* add extra methods here */
};

#define MY_OBJECT(obj)            GTS_OBJECT_CAST (obj,\
					           MyObject,\
					           my_object_class ())
#define MY_OBJECT_CLASS(klass)    GTS_OBJECT_CLASS_CAST (klass,\
						         MyObjectClass,\
						         my_object_class())
#define IS_MY_OBJECT(obj)         (gts_object_is_from_class (obj,\
						   my_object_class ()))
     
MyObjectClass * my_object_class                (void);
MyObject *      my_object_new                  (MyObjectClass * klass);

/* MyObject: Object */

static void my_object_destroy (GtsObject * object)
{
  /* do object-specific cleanup here */
  
  /* do not forget to call destroy method of the parent */
  (* GTS_OBJECT_CLASS (my_object_class ())->parent_class->destroy) (object);
}

static void my_object_class_init (MyObjectClass * klass)
{
  /* define new methods and overload inherited methods here */

  /* example of overloading of the destroy() function 
   (not needed if you don't need to do object-specific cleanup) */
  GTS_OBJECT_CLASS (klass)->destroy = my_object_destroy;
}

static void my_object_init (MyObject * object)
{
  /* initialize object here */
}

MyObjectClass * my_object_class (void)
{
  static MyObjectClass * klass = NULL;

  if (klass == NULL) {
    GtsObjectClassInfo my_object_info = {
      "MyObject",
      sizeof (MyObject),
      sizeof (MyObjectClass),
      (GtsObjectClassInitFunc) my_object_class_init,
      (GtsObjectInitFunc) my_object_init,
      (GtsArgSetFunc) NULL,
      (GtsArgGetFunc) NULL
    };
    klass = gts_object_class_new (GTS_OBJECT_CLASS (parent_class ()),
				  &my_object_info);
  }

  return klass;
}

MyObject * my_object_new (MyObjectClass * klass)
{
  MyObject * object;

  object = MY_OBJECT (gts_object_new (GTS_OBJECT_CLASS (klass)));

  return object;
}
