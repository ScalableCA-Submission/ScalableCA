echo "c now call SamplingCA to generate an initial 2-wise CA"
./SamplingCA/SamplingCA -seed $1 -input_cnf_path $2 -output_testcase_path ./SamplingCA/$3
echo "c now call ScalableCA to generate a 3-wise CA"
./ScalableCA -seed $1 -input_cnf_path $2 -init_2wiseCA_file_path ./SamplingCA/$3 -output_testcase_path $3
rm ./SamplingCA/$3
