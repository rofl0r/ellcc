/** On reset initialization code.
 * This function is as soon as possible after the processor is reset
 * but before the _on_bootstrap() function and C startup code is called.
 * This default version does nothing and can be over-ridden by an application
 * function.
 */
void __attribute__((weak)) _on_reset(void)
{
}
