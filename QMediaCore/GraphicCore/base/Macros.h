//
//  Macros.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_MACROS_H
#define GRAPHICCORE_MACROS_H

/** @def DEPRECATED_ATTRIBUTE
 * Only certain compilers support __attribute__((deprecated)).
 */
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#define DEPRECATED_ATTRIBUTE __attribute__((deprecated))
#elif _MSC_VER >= 1400 //vs 2005 or higher
#define DEPRECATED_ATTRIBUTE __declspec(deprecated)
#else
#define DEPRECATED_ATTRIBUTE
#endif

/** @def DEGREES_TO_RADIANS
 converts degrees to radians
 */
#define DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) * 0.01745329252f) // PI / 180

/** @def RADIANS_TO_DEGREES
 converts radians to degrees
 */
#define RADIANS_TO_DEGREES(__ANGLE__) ((__ANGLE__) * 57.29577951f) // PI * 180


#define GRAPHICCORE_BEGIN namespace GraphicCore {
#define GRAPHICCORE_END }
#define USING_GRAPHICCORE using namespace GraphicCore;

#endif /* GRAPHICCORE_MACROS_H */
