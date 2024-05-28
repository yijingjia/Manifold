import os
import subprocess
import time
import logging

def setup_logger(log_file):
    logger = logging.getLogger()
    logger.setLevel(logging.INFO)
    
    # Create file handler to log to a file
    handler = logging.FileHandler(log_file, mode='w')
    handler.setLevel(logging.INFO)
    
    # Create a logging format
    formatter = logging.Formatter('%(asctime)s - %(message)s')
    handler.setFormatter(formatter)
    
    # Add the handlers to the logger
    logger.addHandler(handler)
    return logger

def process_obj_files(input_dir, output_dir, executable, logger, resolution=20000, sharp=False):
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    total_start_time = time.time()
    
    for root, dirs, files in os.walk(input_dir):
        for file_name in files:
            if file_name.endswith('.obj'):
                input_file = os.path.join(root, file_name)
                relative_path = os.path.relpath(root, input_dir)
                output_subdir = os.path.join(output_dir, relative_path)
                if not os.path.exists(output_subdir):
                    os.makedirs(output_subdir)

                output_file = os.path.join(output_subdir, file_name.replace('.obj', '_manifold.obj'))
                cmd = [executable, input_file, output_file, str(resolution)]
                if sharp:
                    cmd.append('-s')
                
                start_time = time.time()
                try:
                    subprocess.run(cmd, check=True)
                    end_time = time.time()
                    logger.info(f"Processed {input_file} in {end_time - start_time:.2f} seconds")
                except subprocess.CalledProcessError as e:
                    end_time = time.time()
                    logger.error(f"Error processing {input_file}: {e}. Time taken: {end_time - start_time:.2f} seconds")
    
    total_end_time = time.time()
    logger.info(f"Total processing time: {total_end_time - total_start_time:.2f} seconds")


input_dir = "./convertedModelNet10"
output_dir = "./outputModelNet10"
executable = "../Manifold/build/manifold"
log_file = "processing_log.txt"
logger = setup_logger(log_file)

process_obj_files(input_dir, output_dir, executable, logger)

