from string import Template

def generate_config_file(
    farm_id: str,
    lora_addr: int,
    lora_passwd: str,
    comm_type: str
):

    d = {
        'farm_id': f'"{farm_id}"',
        'lora_addr': f'{lora_addr}',
        'lora_passwd': f'"{lora_passwd}"',
        'comm_type': comm_type,
    }

    result = ''

    with open('config_template.h', 'r') as file:
        file_content_string = file.read()

        result = Template(file_content_string).substitute(d)

    with open('config.h', 'w') as file:
        file.write(result)