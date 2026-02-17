#include "texture.h"
#include <GL/freeglut.h>
#include "FreeImage.h"
#include <iostream>

Texture::Texture(const std::string& name, const std::string& filepath)
   : Object(name), m_filepath(filepath), m_texId(0)
{
   // 1. Genera un ID texture OpenGL
   glGenTextures(1, &m_texId);
   glBindTexture(GL_TEXTURE_2D, m_texId);

   // 2. Carica l'immagine con FreeImage
   // Determina il formato (JPEG, PNG, BMP...)
   FREE_IMAGE_FORMAT format = FreeImage_GetFileType(m_filepath.c_str(), 0);
   if (format == FIF_UNKNOWN) {
      format = FreeImage_GetFIFFromFilename(m_filepath.c_str());
   }

   if (format == FIF_UNKNOWN) {
      std::cerr << "[Texture] Error: Unknown file format for " << m_filepath << std::endl;
      return;
   }

   // Carica in memoria
   FIBITMAP* bitmap = FreeImage_Load(format, m_filepath.c_str());
   if (!bitmap) {
      std::cerr << "[Texture] Error: Failed to load " << m_filepath << std::endl;
      return;
   }

   // 3. Converti in 32 bit (RGBA) per compatibilità OpenGL
   FIBITMAP* pImage = FreeImage_ConvertTo32Bits(bitmap);
   // Scarica la bitmap originale non più necessaria
   FreeImage_Unload(bitmap);

   if (pImage) {
      // perchè immagini sono in DDS
      FreeImage_FlipVertical(pImage);

      int nWidth = FreeImage_GetWidth(pImage);
      int nHeight = FreeImage_GetHeight(pImage);

      // 4. Carica i dati nella GPU
      // FreeImage usa BGRA -> usiamo GL_BGRA_EXT
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nWidth, nHeight, 0,
         GL_BGRA_EXT, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(pImage));

      // Imposta filtri di base (necessari per vedere la texture)
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      // Clamping per evitare ripetizioni strane sui bordi
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

      std::cout << "[Texture] Loaded: " << m_filepath << " (ID: " << m_texId << ")" << std::endl;

      FreeImage_Unload(pImage);
   }
   else {
      std::cerr << "[Texture] Error: Failed to convert to 32 bits " << m_filepath << std::endl;
   }

   // Slega la texture
   glBindTexture(GL_TEXTURE_2D, 0);
}
Texture::~Texture() {
	if (m_texId != 0)
		glDeleteTextures(1, &m_texId);
}
void Texture::render() {
	if (m_texId != 0)
		glBindTexture(GL_TEXTURE_2D, m_texId);
}