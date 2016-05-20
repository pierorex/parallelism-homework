#include <errno.h>
#include <sys/time.h>
#include <time.h>
int new_sleep (int amount)
{
        struct timeval Requested, Remaining;
        double famount = amount;
        int rc;
        while (famount > 0.0) {
                Requested.tv_sec = (int) famount;
                Requested.tv_usec =
                        (int) ((famount - Requested.tv_sec)*1000000000.);
                rc = nsleep ( &Requested, &Remaining );
                if ((rc == -1) && (errno == EINTR)) {
                        /* Sleep interrupted.  Resume it */
                        famount = Remaining.tv_sec + Remaining.tv_usec /
                                                        1000000000.;
                        continue;
                }
                else /* Completed sleep.  Set return to zero */
                {
                                return (0);
                }
        }    /* end of while */

        /* famount = 0.; exit */
        return (0);
}
