#include "job_control.h"

/* Mark a stopped job J as being running again.  */

void	mark_job_as_running(t_job *j)
{
	t_process *p;
	p = j->first_process;
	while (p)
	{
		p->stopped = 0;
		p = p->next;
	}
	j->notified = 0;
}

/* Continue the job J.  */

void	continue_job(t_job *j, int foreground)
{
	mark_job_as_running(j);
	if (foreground)
		put_job_in_foreground(singleton_jc(), j, 1);
	else
		put_job_in_background(j, 1);
}
