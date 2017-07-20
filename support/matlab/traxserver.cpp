
#include <stdio.h>
#include <string.h>
#include <string>
#include <list>
#include <algorithm>
#include <cmath>

#if defined(__OS2__) || defined(__WINDOWS__) || defined(WIN32) || defined(WIN64) || defined(_MSC_VER)
#include <io.h>
#else
#include <unistd.h>
#include <errno.h>
#endif
#include <fcntl.h>
#include <ctype.h>

#include "helpers.h"

using namespace std;
using namespace trax;

// Server* handle = NULL;
std::list<Server*> handles;


int handle_is_valid(const Server* handle) {

    std::list<Server*>::iterator iter = std::find(handles.begin(), handles.end(), handle);

    return handles.end() != iter;

}

int fd_is_valid(int fd) {
#if defined(__OS2__) || defined(__WINDOWS__) || defined(WIN32) || defined(WIN64) || defined(_MSC_VER)
    return _tell(fd) != -1 || errno != EBADF;
#else
    return fcntl(fd, F_GETFD) != -1 || errno != EBADF;
#endif
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

    if ( nrhs < 1 ) { MEX_ERROR("At least one input argument required."); return; }

    if (! mxIsChar (prhs[0]) || mxGetNumberOfDimensions (prhs[0]) > 2) { MEX_ERROR("First argument must be a string"); return; }

    string operation = get_string(prhs[0]);

	std::transform(operation.begin(), operation.end(), operation.begin(), ::tolower);

    Server* handle = get_handle(prhs[1]);

    if (operation != "setup" && !handle_is_valid(handle)) {
        MEX_ERROR("Invalid handle.");
        return;
    }

    if (operation == "setup") {

        if ( nrhs < 3 ) { MEX_ERROR("At least three parameters required."); return; }

        if ( nlhs > 1 ) { MEX_ERROR("At most one output argument supported."); return; }

        std::string tracker_name, tracker_description, tracker_family;

        if ( nrhs > 3 ) {

            for (int i = 3; i < std::floor((float)nrhs/2) * 2; i+=2) {
                switch (get_argument_code(get_string(prhs[i]))) {
                case ARGUMENT_TRACKERNAME: tracker_name = get_string(prhs[i+1]); break;
                case ARGUMENT_TRACKERDESCRIPTION: tracker_description = get_string(prhs[i+1]); break;
                case ARGUMENT_TRACKERFAMILY: tracker_family = get_string(prhs[i+1]); break;
                default:
                    MEX_ERROR("Illegal argument.");
                    return;
                }
            }

        }

        int region_formats = get_flags(prhs[1], get_region_code);
        int image_formats = get_flags(prhs[2], get_image_code);

        Metadata metadata(region_formats, image_formats,
            tracker_name, tracker_description, tracker_family);

#if defined(__OS2__) || defined(__WINDOWS__) || defined(WIN32) || defined(WIN64) || defined(_MSC_VER)

        if (!getenv("TRAX_SOCKET")) {
            MEX_ERROR("Socket information not available. Enable socket communication in TraX client.");
            return;
        }

#endif

        handle = new Server(metadata, trax_no_log);
        handles.push_back(handle);

        if (nlhs == 1) {
            plhs[0] = set_handle(handle);
        }

    } else if (operation == "wait") {

		bool propstruct = false;

        if ( nrhs > 1 ) {

			for (int i = 2; i < std::floor((float)nrhs/2) * 2; i+=2) {
				switch (get_argument_code(get_string(prhs[i]))) {
				case ARGUMENT_PROPSTRUCT: propstruct = mxGetScalar(prhs[i+1]) > 0; break;
				default:
					MEX_ERROR("Illegal argument.");
					return;
				}
			}
		}


        if ( nlhs > 3 ) { MEX_ERROR("At most three output argument supported."); return; }

        Image img;
        Region reg;
        Properties prop;

        int tr = handle->wait(img, reg, prop);

        if (tr == TRAX_INITIALIZE) {
            if (nlhs > 0) plhs[0] = image_to_array(img);
            if (nlhs > 1) plhs[1] = region_to_array(reg);
            if (nlhs > 2) plhs[2] = (propstruct) ? parameters_to_struct(prop) : parameters_to_cell(prop);

        } else if (tr == TRAX_FRAME) {

            if (nlhs > 0) plhs[0] = image_to_array(img);
            if (nlhs > 1) plhs[1] = MEX_CREATE_EMTPY;
            if (nlhs > 2) plhs[2] = (propstruct) ? parameters_to_struct(prop) : parameters_to_cell(prop);

        } else {

            if (nlhs > 0) plhs[0] = MEX_CREATE_EMTPY;
            if (nlhs > 1) plhs[1] = MEX_CREATE_EMTPY;
            if (nlhs > 2) plhs[2] = MEX_CREATE_EMTPY;

            if (handle) delete handle;
			handle = NULL;

        }

    } else if (operation == "status") {

        if ( nrhs > 4 ) { MEX_ERROR("Too many parameters."); return; }

        if ( nrhs < 3 ) { MEX_ERROR("Must specify region parameter."); return; }

        if ( nlhs > 1 ) { MEX_ERROR("One output parameter allowed."); return; }

        if (!MEX_TEST_DOUBLE(1)) { MEX_ERROR("Illegal region format."); return; }

        Region reg = array_to_region(prhs[2]);

        if (!reg) { MEX_ERROR("Illegal region format."); return; }

        Properties prop = ( nrhs == 3 ) ? struct_to_parameters(prhs[3]) : Properties();

        handle->reply(reg, prop);

        if (nlhs == 1)
            plhs[0] = mxCreateLogicalScalar(handle > 0);

    } else if (operation == "quit") {

		delete handle;
		handle = NULL;

    } else {
        MEX_ERROR("Unknown operation.");
    }

}

