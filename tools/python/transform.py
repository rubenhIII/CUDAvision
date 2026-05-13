import cv2
import numpy as np
import os

# =========================
# Leer imagen a color
# =========================
img_folder = "../../img"
path = os.path.join(img_folder, "lena.jpeg")

# Leer en color
img = cv2.imread(path, cv2.IMREAD_COLOR)

if img is None:
    raise Exception("No se pudo leer lena.jpeg")

# =========================
# Transformación 1:
# Escalado conservando tamaño original
# =========================

scale = 0.75

h, w = img.shape[:2]

# Reducir imagen
scaled = cv2.resize(
    img,
    None,
    fx=scale,
    fy=scale,
    interpolation=cv2.INTER_LINEAR
)

# Crear canvas negro del tamaño original
scaled_back = np.zeros_like(img)

hs, ws = scaled.shape[:2]

# Centrar imagen escalada
x_offset = (w - ws) // 2
y_offset = (h - hs) // 2

scaled_back[
    y_offset:y_offset + hs,
    x_offset:x_offset + ws
] = scaled

# =========================
# Transformación 2:
# Rotación
# =========================

center = (w // 2, h // 2)

angle = 15  # grados

M = cv2.getRotationMatrix2D(center, angle, 1.0)

rotated = cv2.warpAffine(
    img,
    M,
    (w, h),
    borderMode=cv2.BORDER_CONSTANT,
    borderValue=(0, 0, 0)
)

# =========================
# Transformación 3:
# Traslación
# =========================

tx = 20
ty = 15

T = np.float32([
    [1, 0, tx],
    [0, 1, ty]
])

translated = cv2.warpAffine(
    img,
    T,
    (w, h),
    borderMode=cv2.BORDER_CONSTANT,
    borderValue=(0, 0, 0)
)

# =========================
# Guardar resultados
# =========================

cv2.imwrite(os.path.join(img_folder, "lena_scaled.jpeg"), scaled_back)
cv2.imwrite(os.path.join(img_folder, "lena_rotated.jpeg"), rotated)
cv2.imwrite(os.path.join(img_folder, "lena_translated.jpeg"), translated)

print("Imágenes guardadas:")
print("- lena_scaled.jpeg")
print("- lena_rotated.jpeg")
print("- lena_translated.jpeg")