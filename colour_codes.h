#ifndef COLORS_H_
#define COLORS_H_

#define BOLD       "\033[1m"		// жирный шрифт (интенсивный цвет)
#define DBOLD      "\033[2m"		// полу яркий цвет (тёмно-серый, независимо от цвета)
#define NBOLD      "\033[22m"		// установить нормальную интенсивность
#define UNDERLINE  "\033[4m"		// подчеркивание
#define NUNDERLINE "\033[4m"	    // отменить подчеркивание
#define BLINK      "\033[5m"	    // мигающий
#define NBLINK     "\033[0m"		// отменить мигание
#define INVERSE    "\033[7m"		// реверсия (знаки приобретают цвет фона, а фон -- цвет знаков)
#define NINVERSE   "\033[7m"		// отменить реверсию
#define BREAK      "\033[m"			// все атрибуты по умолчанию

#define RESET      "\033[0m"
#define BLACK      "\033[30m"
#define RED        "\033[31m"
#define GREEN      "\033[32m"
#define YELLOW     "\033[33m"
#define BLUE       "\033[34m"
#define MAGENTA    "\033[35m"
#define CYAN       "\033[36m"
#define GRAY       "\033[37m"

#define BGBLACK    "\033[40m"
#define BGRED      "\033[41m"
#define BGGREEN    "\033[42m"
#define BGBROWN    "\033[43m"
#define BGBLUE     "\033[44m"
#define BGMAGENTA  "\033[45m"
#define BGCYAN     "\033[46m"
#define BGGRAY     "\033[47m"
#define BGDEF      "\033[49m"

#endif  // COLORS_H_
