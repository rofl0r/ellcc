/** On bootstrap initialization code.
 * This function is called after the the processor has been initialized
 * but before the C startup code is called. This default version does
 * nothing and can be over-ridden by an application function.
 */
void __attribute__((weak)) _on_bootstrap(void)
{
}
