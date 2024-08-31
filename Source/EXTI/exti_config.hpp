// gd32f30x EXTI peripheral register access in C++
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdlib>
#include <cstdint>

#include "CONFIG.hpp"

namespace exti {


///////////////////////////// REGISTER OFFSETS /////////////////////////////

enum class EXTI_Regs {
    INTEN = 0x00,
    EVEN = 0x04,
    RTEN = 0x08,
    FTEN = 0x0C,
    SWIEV = 0x10,
    PD = 0x14
};


///////////////////////////// REGISTER BITS /////////////////////////////

enum class INTEN_Bits {
    INTEN0 = REG_BIT_DEF(0, 0),
    INTEN1 = REG_BIT_DEF(1, 1),
    INTEN2 = REG_BIT_DEF(2, 2),
    INTEN3 = REG_BIT_DEF(3, 3),
    INTEN4 = REG_BIT_DEF(4, 4),
    INTEN5 = REG_BIT_DEF(5, 5),
    INTEN6 = REG_BIT_DEF(6, 6),
    INTEN7 = REG_BIT_DEF(7, 7),
    INTEN8 = REG_BIT_DEF(8, 8),
    INTEN9 = REG_BIT_DEF(9, 9),
    INTEN10 = REG_BIT_DEF(10, 10),
    INTEN11 = REG_BIT_DEF(11, 11),
    INTEN12 = REG_BIT_DEF(12, 12),
    INTEN13 = REG_BIT_DEF(13, 13),
    INTEN14 = REG_BIT_DEF(14, 14),
    INTEN15 = REG_BIT_DEF(15, 15),
    INTEN16 = REG_BIT_DEF(16, 16),
    INTEN17 = REG_BIT_DEF(17, 17),
    INTEN18 = REG_BIT_DEF(18, 18),
    INTEN19 = REG_BIT_DEF(19, 19)
};

enum class EVEN_Bits {
    EVEN0 = REG_BIT_DEF(0, 0),
    EVEN1 = REG_BIT_DEF(1, 1),
    EVEN2 = REG_BIT_DEF(2, 2),
    EVEN3 = REG_BIT_DEF(3, 3),
    EVEN4 = REG_BIT_DEF(4, 4),
    EVEN5 = REG_BIT_DEF(5, 5),
    EVEN6 = REG_BIT_DEF(6, 6),
    EVEN7 = REG_BIT_DEF(7, 7),
    EVEN8 = REG_BIT_DEF(8, 8),
    EVEN9 = REG_BIT_DEF(9, 9),
    EVEN10 = REG_BIT_DEF(10, 10),
    EVEN11 = REG_BIT_DEF(11, 11),
    EVEN12 = REG_BIT_DEF(12, 12),
    EVEN13 = REG_BIT_DEF(13, 13),
    EVEN14 = REG_BIT_DEF(14, 14),
    EVEN15 = REG_BIT_DEF(15, 15),
    EVEN16 = REG_BIT_DEF(16, 16),
    EVEN17 = REG_BIT_DEF(17, 17),
    EVEN18 = REG_BIT_DEF(18, 18),
    EVEN19 = REG_BIT_DEF(19, 19)
};

enum class RTEN_Bits {
    RTEN0 = REG_BIT_DEF(0, 0),
    RTEN1 = REG_BIT_DEF(1, 1),
    RTEN2 = REG_BIT_DEF(2, 2),
    RTEN3 = REG_BIT_DEF(3, 3),
    RTEN4 = REG_BIT_DEF(4, 4),
    RTEN5 = REG_BIT_DEF(5, 5),
    RTEN6 = REG_BIT_DEF(6, 6),
    RTEN7 = REG_BIT_DEF(7, 7),
    RTEN8 = REG_BIT_DEF(8, 8),
    RTEN9 = REG_BIT_DEF(9, 9),
    RTEN10 = REG_BIT_DEF(10, 10),
    RTEN11 = REG_BIT_DEF(11, 11),
    RTEN12 = REG_BIT_DEF(12, 12),
    RTEN13 = REG_BIT_DEF(13, 13),
    RTEN14 = REG_BIT_DEF(14, 14),
    RTEN15 = REG_BIT_DEF(15, 15),
    RTEN16 = REG_BIT_DEF(16, 16),
    RTEN17 = REG_BIT_DEF(17, 17),
    RTEN18 = REG_BIT_DEF(18, 18),
    RTEN19 = REG_BIT_DEF(19, 19),
};

enum class FTEN_Bits {
    FTEN0 = REG_BIT_DEF(0, 0),
    FTEN1 = REG_BIT_DEF(1, 1),
    FTEN2 = REG_BIT_DEF(2, 2),
    FTEN3 = REG_BIT_DEF(3, 3),
    FTEN4 = REG_BIT_DEF(4, 4),
    FTEN5 = REG_BIT_DEF(5, 5),
    FTEN6 = REG_BIT_DEF(6, 6),
    FTEN7 = REG_BIT_DEF(7, 7),
    FTEN8 = REG_BIT_DEF(8, 8),
    FTEN9 = REG_BIT_DEF(9, 9),
    FTEN10 = REG_BIT_DEF(10, 10),
    FTEN11 = REG_BIT_DEF(11, 11),
    FTEN12 = REG_BIT_DEF(12, 12),
    FTEN13 = REG_BIT_DEF(13, 13),
    FTEN14 = REG_BIT_DEF(14, 14),
    FTEN15 = REG_BIT_DEF(15, 15),
    FTEN16 = REG_BIT_DEF(16, 16),
    FTEN17 = REG_BIT_DEF(17, 17),
    FTEN18 = REG_BIT_DEF(18, 18),
    FTEN19 = REG_BIT_DEF(19, 19)
};

enum class SWIEV_Bits {
    SWIEV0 = REG_BIT_DEF(0, 0),
    SWIEV1 = REG_BIT_DEF(1, 1),
    SWIEV2 = REG_BIT_DEF(2, 2),
    SWIEV3 = REG_BIT_DEF(3, 3),
    SWIEV4 = REG_BIT_DEF(4, 4),
    SWIEV5 = REG_BIT_DEF(5, 5),
    SWIEV6 = REG_BIT_DEF(6, 6),
    SWIEV7 = REG_BIT_DEF(7, 7),
    SWIEV8 = REG_BIT_DEF(8, 8),
    SWIEV9 = REG_BIT_DEF(9, 9),
    SWIEV10 = REG_BIT_DEF(10, 10),
    SWIEV11 = REG_BIT_DEF(11, 11),
    SWIEV12 = REG_BIT_DEF(12, 12),
    SWIEV13 = REG_BIT_DEF(13, 13),
    SWIEV14 = REG_BIT_DEF(14, 14),
    SWIEV15 = REG_BIT_DEF(15, 15),
    SWIEV16 = REG_BIT_DEF(16, 16),
    SWIEV17 = REG_BIT_DEF(17, 17),
    SWIEV18 = REG_BIT_DEF(18, 18),
    SWIEV19 = REG_BIT_DEF(19, 19),
};

enum class PD_Bits {
    PD0 = REG_BIT_DEF(0, 0),
    PD1 = REG_BIT_DEF(1, 1),
    PD2 = REG_BIT_DEF(2, 2),
    PD3 = REG_BIT_DEF(3, 3),
    PD4 = REG_BIT_DEF(4, 4),
    PD5 = REG_BIT_DEF(5, 5),
    PD6 = REG_BIT_DEF(6, 6),
    PD7 = REG_BIT_DEF(7, 7),
    PD8 = REG_BIT_DEF(8, 8),
    PD9 = REG_BIT_DEF(9, 9),
    PD10 = REG_BIT_DEF(10, 10),
    PD11 = REG_BIT_DEF(11, 11),
    PD12 = REG_BIT_DEF(12, 12),
    PD13 = REG_BIT_DEF(13, 13),
    PD14 = REG_BIT_DEF(14, 14),
    PD15 = REG_BIT_DEF(15, 15),
    PD16 = REG_BIT_DEF(16, 16),
    PD17 = REG_BIT_DEF(17, 17),
    PD18 = REG_BIT_DEF(18, 18),
    PD19 = REG_BIT_DEF(19, 19),
};

enum class EXTI_Line {
    EXTI0 = REG_BIT_DEF(0, 0),
    EXTI1 = REG_BIT_DEF(1, 1),
    EXTI2 = REG_BIT_DEF(2, 2),
    EXTI3 = REG_BIT_DEF(3, 3),
    EXTI4 = REG_BIT_DEF(4, 4),
    EXTI5 = REG_BIT_DEF(5, 5),
    EXTI6 = REG_BIT_DEF(6, 6),
    EXTI7 = REG_BIT_DEF(7, 7),
    EXTI8 = REG_BIT_DEF(8, 8),
    EXTI9 = REG_BIT_DEF(9, 9),
    EXTI10 = REG_BIT_DEF(10, 10),
    EXTI11 = REG_BIT_DEF(11, 11),
    EXTI12 = REG_BIT_DEF(12, 12),
    EXTI13 = REG_BIT_DEF(13, 13),
    EXTI14 = REG_BIT_DEF(14, 14),
    EXTI15 = REG_BIT_DEF(15, 15),
    EXTI16 = REG_BIT_DEF(16, 16),
    EXTI17 = REG_BIT_DEF(17, 17),
    EXTI18 = REG_BIT_DEF(18, 18),
    EXTI19 = REG_BIT_DEF(19, 19),
};


///////////////////////////// ENUMS /////////////////////////////

enum class EXTI_Mode {
    EXTI_INTERRUPT,
    EXTI_EVENT
};

enum class EXTI_Trigger {
    TRIG_RISING = 0,
    TRIG_FALLING,
    TRIG_BOTH,
    TRIG_NONE
};

} // namespace exti
