import os
import json

CONFIG_FILE = "i2c_config.json"
DEFAULT_CONFIG = {
    "port": "COM10",
    "slave_addr": "0x52",
    "iic_speed": "100k"
}

def load_config():
    if os.path.exists(CONFIG_FILE):
        try:
            with open(CONFIG_FILE, 'r', encoding='utf-8') as f:
                return json.load(f)
        except:
            pass
    return DEFAULT_CONFIG.copy()

def save_config(port, slave_addr, iic_speed=None):
    config = {
        "port": str(port).strip(),
        "slave_addr": str(slave_addr).strip()
    }
    if iic_speed is not None:
        config["iic_speed"] = str(iic_speed).strip()
    try:
        with open(CONFIG_FILE, 'w', encoding='utf-8') as f:
            json.dump(config, f, indent=4)
    except Exception as e:
        print(f"保存配置失败: {e}")
