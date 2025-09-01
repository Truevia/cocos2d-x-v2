# safe_file_id_base62.py
import hashlib
import re

BASE62_ALPHABET = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"

def normalize_path(path: str) -> str:
    """规范化路径：统一 / 分隔符，去掉重复的 //，去掉前导 ./"""
    p = path.replace("\\", "/")
    p = re.sub(r"/{2,}", "/", p)
    if p.startswith("./"):
        p = p[2:]
    return p

def int_to_base62(num: int) -> str:
    """把整数转成 Base62 字符串"""
    if num == 0:
        return BASE62_ALPHABET[0]
    chars = []
    while num > 0:
        num, rem = divmod(num, 62)
        chars.append(BASE62_ALPHABET[rem])
    return "".join(reversed(chars))

def short_id_from_path(path: str, bits: int = 80, algo: str = "sha256") -> str:
    """
    根据路径生成 Base62 短ID
    - algo: 哈希算法（sha256/md5/blake2b）
    - bits: 截断位数，建议 64/80/96
    返回 Base62 字符串
    """
    if bits % 8 != 0 or bits <= 0:
        raise ValueError("bits 必须是 8 的倍数（如 64/80/96）")
    norm = normalize_path(path).encode("utf-8")

    if algo == "sha256":
        digest = hashlib.sha256(norm).digest()
    elif algo == "md5":
        digest = hashlib.md5(norm).digest()
    elif algo == "blake2b":
        digest = hashlib.blake2b(norm, digest_size=32).digest()
    else:
        raise ValueError("不支持的算法")

    truncated = digest[: bits // 8]
    num = int.from_bytes(truncated, "big")
    return int_to_base62(num)

# --------- 测试 ----------
if __name__ == "__main__":
    path = "res/font/test.fnt"
    id64 = short_id_from_path(path, bits=64)
    id80 = short_id_from_path(path, bits=80)
    id96 = short_id_from_path(path, bits=96)
    print("64bit:", id64)
    print("80bit:", id80)
    print("96bit:", id96)