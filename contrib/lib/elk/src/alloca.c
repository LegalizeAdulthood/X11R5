#include "scheme.h"

#ifndef PORTABLE
 #error "You must define PORTABLE in src/config when MACHTYPE=port"
#endif

unwind_free (uwp) struct unwind_info *uwp; {
    register struct unwind_info *p_, *q_;

    for (p_ = uwp; p_; p_ = q_) {
	if (!(q_ = p_->next))
	    copy_jmpbuf (p_->last_target, last_jmpbuf);
	free ((char *)p_);
    }
}
