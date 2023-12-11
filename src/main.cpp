#include "Optimizer.h"
#include <iostream>
#include <signal.h>
#include <chrono>
#include <cstring>
#include <string>
using namespace std;

void HandleInterrupt(int sig){
    cout << "c" << endl;
    cout << "c caught signal... exiting" << endl;

    exit(-1);
}

void SetupSignalHandler(){
    signal(SIGTERM, HandleInterrupt);
    signal(SIGINT, HandleInterrupt);
    signal(SIGQUIT, HandleInterrupt);
    signal(SIGKILL, HandleInterrupt);
}

bool ParseArgument(int argc, char **argv, Argument &argu) {
    argu.seed = 1;
    argu.candidate_set_size = 100;
    argu.use_cnf_reduction = true;
    argu.flag_input_cnf_path = false;
    argu.flag_init_PCA_file_path = false;
    argu.flag_output_testcase_path = false;
    
    argu.use_cache = 1;
    argu.use_invalid_expand = 1;
    argu.use_context_aware = false;
    argu.use_pbo_solver = false;
    argu.uniform_sampling_greedy = false;
    argu.uniform_sampling = false;

    argu.stop_length = 500;

    argu.use_RALS = true;

    for (int i = 1; i < argc; i ++) {
        if (strcmp(argv[i], "-input_cnf_path") == 0) {
			i ++;
			if(i >= argc) return false;
			argu.input_cnf_path = argv[i];
            argu.flag_input_cnf_path = true;
			continue ;
		}
        else if (strcmp(argv[i], "-init_2wiseCA_file_path") == 0) {
			i ++;
			if(i >= argc) return false;
			argu.init_PCA_file_path = argv[i];
            argu.flag_init_PCA_file_path = true;
			continue ;
		}
        else if (strcmp(argv[i], "-output_testcase_path") == 0) {
			i ++;
			if(i >= argc) return false;
			argu.output_testcase_path = argv[i];
            argu.flag_output_testcase_path = true;
			continue ;
		}
        else if (strcmp(argv[i], "-seed") == 0) {
			i ++;
			if(i >= argc) return false;
			sscanf(argv[i], "%d", &argu.seed);
			continue ;
		}
        else if(strcmp(argv[i], "-delta") == 0) {
			i ++;
			if(i >= argc) return false;
			sscanf(argv[i], "%d", &argu.candidate_set_size);
			continue ;
		}
        else if(strcmp(argv[i], "-cache") == 0) {
            i ++;
            if(i >= argc) return false;
            sscanf(argv[i], "%d", &argu.use_cache);
            continue ;
        }

        else if(strcmp(argv[i], "-L") == 0) {
            i ++;
            if(i >= argc) return false;
            sscanf(argv[i], "%d", &argu.stop_length);
            continue ;
        }

        else if(strcmp(argv[i], "-use_fid") == 0) {
            i ++;
            if(i >= argc) return false;
            sscanf(argv[i], "%d", &argu.use_invalid_expand);
            continue ;
        }

        else if(strcmp(argv[i], "-use_RALS") == 0) {
            i ++;
            if(i >= argc) return false;
            sscanf(argv[i], "%d", &argu.use_RALS);
            continue ;
        }

        else if(strcmp(argv[i], "-use_context_aware") == 0) {
            argu.use_context_aware = true;
            argu.use_pbo_solver = true;
            continue ;
        }

        else if(strcmp(argv[i], "-use_pbo_solver") == 0) {
            argu.use_pbo_solver = true;
            continue ;
        }

        else if(strcmp(argv[i], "-uniform_sampling_greedy") == 0) {
            argu.uniform_sampling_greedy = true ;
            continue ;
        }

        else if(strcmp(argv[i], "-uniform_sampling") == 0) {
            argu.uniform_sampling = true;
            continue ;
        }
    }

    if(! argu.flag_input_cnf_path || ! argu.flag_init_PCA_file_path)
        return false;

    if(! argu.flag_output_testcase_path) {
        int pos = argu.input_cnf_path.find_last_of( '/' );
        string cnf_file_name = argu.input_cnf_path.substr(pos + 1);
	    cnf_file_name.replace(cnf_file_name.find(".cnf"), 4, "");

        argu.output_testcase_path = cnf_file_name + "_testcase.out";
    }
    
    return true;
}

int main(int argc, char **argv) {
    SetupSignalHandler();
    
    Argument argu;

	if (! ParseArgument(argc, argv, argu)) {
		cout << "c Argument Error!" << endl;
		return -1;
	}

    int stop_length = argu.stop_length;

    Expandor expandor(argu);
    expandor.GenerateCoveringArray();

    int nvar, nclauses;
    vector<vector<int> > clauses;
    vector<vector<int> > _2PCA_testcase;
    vector<vector<int> > init_testcase;
    vector<_3tuples> tuples_U;
    vector<vector<int> > pos_in_cls;
    vector<vector<int> > neg_in_cls;

    expandor.dump(nvar, nclauses, clauses, _2PCA_testcase, init_testcase, 
        tuples_U, pos_in_cls, neg_in_cls);
    
    expandor.Free();

    if(! argu.use_RALS) {
        cout << "End" << endl;
        return 0;
    }

    Optimizer optimizer(argu.seed, nvar, nclauses, clauses, 
        _2PCA_testcase, init_testcase, tuples_U,
        argu.output_testcase_path, pos_in_cls, neg_in_cls,
        stop_length);

    vector<vector<int> >().swap(clauses);
    vector<vector<int> >().swap(_2PCA_testcase);
    vector<vector<int> >().swap(init_testcase);
    vector<_3tuples>().swap(tuples_U);
    vector<vector<int> >().swap(pos_in_cls);
    vector<vector<int> >().swap(neg_in_cls);

    optimizer.search();
    cout << "End" << endl;
    return 0;
}