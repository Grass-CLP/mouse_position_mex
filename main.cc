#include <tchar.h>
#include <windows.h>
#include "mex.h"
#include "time.h"
#include "mouse_position.h"

MousePositionCapture &capture = MousePositionCapture::instance();

void mexFunction(int nlhs, mxArray* plhs[],
                 int nrhs, const mxArray* prhs[]) {

	/* Check for proper number of input and output arguments */
	if (nrhs != 1) {
		mexErrMsgIdAndTxt("MATLAB:fastrak:invalidNumInputs",
		                  "One input argument required.");
	}

	/* Check to be sure input is of type char */
	if (!(mxIsChar(prhs[0]))) {
		mexErrMsgIdAndTxt("MATLAB:fastrak:inputNotString",
		                  "Input must be of type string.\n.");
	}

	char* str;
	str = mxArrayToString(prhs[0]);

	if (!strcmp(str, "hifi")) {
		bool hifi = capture.toggleHiFi();
		
		mexPrintf("hifi: " + hifi ? "true\n" : "false\n");
		goto end;
	}

	if (!strncmp(str, "hz ", 3)) {
		string hzStr(str + 3);
		int hz = atoi(str + 3);
		if (hz > 0 && hz < CAPTURE_HZ) {
			capture.setFrequency(hz);
			capture.reset();

			mexPrintf("set frequency to %d hz and reset \n", hz);
		}

		mexPrintf("set frequency failed, hz is too high\n");

		goto end;
	}

	if (!strcmp(str, "start")) {
		mexPrintf("start\n");
		capture.start();
		goto end;
	}

	if (!strcmp(str, "reset")) {
		mexPrintf("reset data\n");
		capture.reset();
		goto end;
	}

	if (!strcmp(str, "getData")) {
		if (nlhs != 1) {
			mexErrMsgIdAndTxt("MATLAB:fastrak:maxlhs",
			                  "One arguments required.");
		}

		int frameCount = capture.captureImage();
		if (frameCount > 0) {
			plhs[0] = mxCreateNumericMatrix(frameCount, 3, mxINT64_CLASS, mxREAL);
			INT64* result = (INT64*)mxGetPr(plhs[0]);

			capture.load(result, frameCount);
		}
		
		mexPrintf("getData\n");
		goto end;
	}

	if (!strcmp(str, "stop")) {
		mexPrintf("stop capture\n");
		capture.stop();
		goto end;
	}

	if (!strcmp(str, "now")) {
		plhs[0] = mxCreateNumericMatrix(1, 1, mxINT64_CLASS, mxREAL);
		INT64* p1 = (INT64*)mxGetPr(plhs[0]);
		*p1 = GetTime();
		goto end;
	}

end:
	mxFree(str);
}
