/*
 *TO ensure proper functioning,
 *plase ensure the libraryes are properly installed
 *Also, all library includes and function
 *are disable by handeling "freertos_custom_defines"
 *
 *Regarding Slack Stealing Library:
 *
 *If your RTOS version (as in this example) is below 10.0.1, do the following:
 *THE FOLLORING IS ONLY VALID IF YOU ARE USING STMCUBEIDE:
 *       STEPS TO CHANGE THIS FLAG MAY CHANCE BASED ON YOUR IDE
 *Go to your proyect properties (right click on proyect folder -> properties)
 *Go C/C++BUild → Settings → MGU GCC linker → Miscellaneous->other flags
 *add the following flag:
 *-Wl,--wrap=vTaskDelayUntil -Wl,--wrap=xTaskIncrementTick
 *
 *And then your proyect should we working fine
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
