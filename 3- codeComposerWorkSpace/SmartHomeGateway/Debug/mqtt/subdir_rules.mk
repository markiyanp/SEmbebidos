################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
mqtt/%.obj: ../mqtt/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/TI/ccs920/ccs/tools/compiler/ti-cgt-arm_18.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O2 --include_path="C:/TI/ccs920/ccs/tools/compiler/ti-cgt-arm_18.12.3.LTS/include" --include_path="C:/TI/tivaware_c_series_2_1_4_178/inc" --include_path="C:/TI/tivaware_c_series_2_1_4_178/driverlib" --include_path="C:/TI/tivaware_c_series_2_1_4_178/utils" --include_path="C:/TI/CC3100SDK_1.3.0/cc3100-sdk/platform/tiva-c-launchpad" --include_path="C:/TI/CC3100SDK_1.3.0/cc3100-sdk/simplelink" --include_path="C:/TI/CC3100SDK_1.3.0/cc3100-sdk/simplelink/include" --include_path="C:/TI/CC3100SDK_1.3.0/cc3100-sdk/simplelink/source" --include_path="C:/Users/Admin/Desktop/TECNICO/1-Semestre/SEMB/IOT Project/3- codeComposerWorkSpace/SmartHomeGateway" --include_path="C:/TI/tivaware_c_series_2_1_4_178/examples/boards/ek-tm4c123gxl" --include_path="C:/TI/tivaware_c_series_2_1_4_178" --include_path="C:/TI/tivaware_c_series_2_1_4_178/third_party" --include_path="C:/TI/tivaware_c_series_2_1_4_178/third_party/FreeRTOS/Source/include" --include_path="C:/TI/tivaware_c_series_2_1_4_178/third_party/FreeRTOS" --include_path="C:/TI/tivaware_c_series_2_1_4_178/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --advice:power=all --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_TM4C123_RB1 -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --ual --preproc_with_compile --preproc_dependency="mqtt/$(basename $(<F)).d_raw" --obj_directory="mqtt" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


