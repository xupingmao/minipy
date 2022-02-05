#include "random.c"

/*
 * time_mod_init()
 *
 * time module initialization function
 */
void time_init(TP)
{
    /*
     * module dict for time
     */
    tp_obj time_mod = tp_dict(tp);

    /*
     * bind functions to random module
     */
    tp_set(tp, time_mod, tp_string("time"),       tp_fnc(tp, time_time));

    /*
     * bind usual distribution random variable generator
     */
    // tp_set(tp, random_mod, tp_string("uniform"),        tp_fnc(tp, random_uniform));

    /*
     * bind special attributes to time module
     */
    tp_set(tp, time_mod, tp_string("__doc__"),  tp_string("Time."));
    tp_set(tp, time_mod, tp_string("__name__"), tp_string("time"));
    tp_set(tp, time_mod, tp_string("__file__"), tp_string(__FILE__));

    /*
     * bind random module to tinypy modules[]
     */
    tp_set(tp, tp->modules, tp_string("time"), time_mod);
}
