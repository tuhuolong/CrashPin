//+----------------------------------------------------------------------------+
//|                                                                            |
//| Copyright(c) 2011, Dmitry "PVOID" Petuhov                                  |
//| All rights reserved.                                                       |
//|                                                                            |
//| Redistribution and use in source and binary forms, with or without         |
//| modification, are permitted provided that the following conditions are     |
//| met:                                                                       |
//|                                                                            |
//|   - Redistributions of source code must retain the above copyright notice, |
//|     this list of conditions and the following disclaimer.                  |
//|   - Redistributions in binary form must reproduce the above copyright      |
//|     notice, this list of conditions and the following disclaimer in the    |
//|     documentation and / or other materials provided with the distribution. |
//|                                                                            |
//| THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS        |
//| "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED  |
//| TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR |
//| PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR          |
//| CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,      |
//| EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO,         |
//| PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR         |
//| PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF     |
//| LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING        |
//| NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS         |
//| SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.               |
//|                                                                            |
//+----------------------------------------------------------------------------+
#include "crashpin.h"
#include <unistd.h>
#include <sys/wait.h>
#include <android/log.h>
#include <sys/ptrace.h>
#include <errno.h>

#include "stacktrace.h"
#include "log.h"
//+----------------------------------------------------------------------------+
//|                                                                            |
//+----------------------------------------------------------------------------+
#ifndef NULL
#define NULL 0
#endif
//+----------------------------------------------------------------------------+
//|                                                                            |
//+----------------------------------------------------------------------------+
#ifndef PR_SET_DUMPABLE
#define PR_SET_DUMPABLE   4
#endif
//+----------------------------------------------------------------------------+
//|                                                                            |
//+----------------------------------------------------------------------------+
#define FAILED(x)  (x)<0
//+----------------------------------------------------------------------------+
//|                                                                            |
//+----------------------------------------------------------------------------+
extern void reportCrash(char* info);
static struct sigaction old_sa[NSIG];

bool SCrashPin::Initialize() {
	struct sigaction handler;
///////
	handler.sa_sigaction = SigactionHandler;
	handler.sa_flags = SA_SIGINFO;
///////
// We need make process dumpable
///////
	if (prctl(PR_SET_DUMPABLE, 1, 0, 0, 0) < 0) {
		LOGE("Can't set process dumpable");
		return (false);
	}
///////
// TODO: Save old handlers
//////
	sigaction(SIGSEGV, &handler, &old_sa[SIGSEGV]);
	sigaction(SIGILL, &handler, &old_sa[SIGILL]);
	sigaction(SIGSTKFLT, &handler, &old_sa[SIGSTKFLT]);
	sigaction(SIGABRT, &handler, &old_sa[SIGABRT]);
	sigaction(SIGBUS, &handler, &old_sa[SIGBUS]);
	sigaction(SIGFPE, &handler, &old_sa[SIGFPE]);
//////
	return (true);
}
//+----------------------------------------------------------------------------+
//|                                                                            |
//+----------------------------------------------------------------------------+
void SCrashPin::SigactionHandler(int sig, siginfo* info, void* reserved) {
	pid_t pid = getpid();
	pid_t tid = gettid();
//  if (old_sa[sig].sa_sigaction)
//  		old_sa[sig].sa_sigaction(sig,info,reserved);
//////////
	switch (fork()) {
	case -1:
		kill(getpid(), SIGKILL);
		return;
	case 0:
		Mosquito(pid, tid);
		kill(getpid(), SIGKILL);
		break;
	default:
		kill(getpid(), SIGSTOP);
		break;
	}
}
//+----------------------------------------------------------------------------+
//|                                                                            |
//+----------------------------------------------------------------------------+
void SCrashPin::Mosquito(pid_t pid, pid_t tid) {
	int status, signal;
////////
// Attaching to process and thread
////////
	if (FAILED(ptrace(PTRACE_ATTACH,pid,0,0))) {
		LOGE("Can't attach to process over ptrace");
		kill(pid, SIGKILL);
		return;
	}
	if (pid != tid && FAILED(ptrace(PTRACE_ATTACH,tid,0,0))) {
		LOGE("Can't attach to thread over ptrace");
		kill(pid, SIGKILL);
		return;
	}
////////
// Catching child signal
////////
	do {
		if (FAILED(waitpid(tid,&status,__WALL))) {
			if (errno == EAGAIN)
				continue;
			break;
		}
////////
		if (WIFSTOPPED(status)) {
			signal = WSTOPSIG(status);
			switch (signal) {
			case SIGSTOP: {
				if (FAILED(ptrace(PTRACE_CONT, tid, 0, 0)))
					break;
				continue;
			}
			case SIGILL:
			case SIGABRT:
			case SIGBUS:
			case SIGFPE:
			case SIGSEGV:
			case SIGSTKFLT: {
				siginfo signal_info;
				ptrace(PTRACE_GETSIGINFO, tid, NULL, &signal_info);

//				LOGE("Process caught signal %d, %d, %d", signal_info.si_signo,
//						signal_info.si_errno, signal_info.si_code);

				CStackTrace trace(tid);
				trace.BackTrace();
				char* dumpInfo = trace.Dump();
//				LOGE("dumpinfo %s", dumpInfo);
				reportCrash(dumpInfo);
				delete dumpInfo;

				kill(pid, SIGKILL);
				kill(getpid(), SIGKILL);
				break;
			}
			}
		}
////////
		break;
	} while (1);
////////
// Killing child process
////////
//	kill(pid, SIGKILL);
}
//+----------------------------------------------------------------------------+
