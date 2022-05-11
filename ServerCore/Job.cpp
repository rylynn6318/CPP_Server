#include "pch.h"
#include "Job.h"

auto Job::Execute() -> void
{
	_callback();
}
