#include "AnalysisCore.h"
#include <algorithm>

namespace pca
{
    AnalysisCore::AnalysisCore (KeyDetector::Perfiles perfiles) : detector (perfiles) {}

    void AnalysisCore::prepare (double sampleRate)
    {
        frecuenciaMuestreo = sampleRate;
        extractor.prepare (sampleRate);

        trama.assign ((size_t) extractor.getFrameSize(), 0.0f);
        salto.assign ((size_t) extractor.getHopSize(), 0.0f);
        muestrasEnSalto = 0;
        muestrasEnTrama = 0;
    }

    void AnalysisCore::reset()
    {
        detector.reset();
        std::fill (trama.begin(), trama.end(), 0.0f);
        muestrasEnSalto = 0;
        muestrasEnTrama = 0;
    }

    void AnalysisCore::pushSamples (const float* muestras, int numMuestras)
    {
        const auto tamSalto = extractor.getHopSize();

        while (numMuestras > 0)
        {
            const auto aCopiar = std::min (numMuestras, tamSalto - muestrasEnSalto);
            std::copy (muestras, muestras + aCopiar, salto.begin() + muestrasEnSalto);

            muestras += aCopiar;
            numMuestras -= aCopiar;
            muestrasEnSalto += aCopiar;

            if (muestrasEnSalto == tamSalto)
            {
                // Desplazar la trama un salto a la izquierda y meter las nuevas al final.
                std::move (trama.begin() + tamSalto, trama.end(), trama.begin());
                std::copy (salto.begin(), salto.end(), trama.end() - tamSalto);
                muestrasEnSalto = 0;

                muestrasEnTrama = std::min (muestrasEnTrama + tamSalto, extractor.getFrameSize());
                if (muestrasEnTrama == extractor.getFrameSize())
                    analizarTrama();
            }
        }
    }

    void AnalysisCore::analizarTrama()
    {
        Cromagrama croma;
        float rms = 0.0f;
        extractor.process (trama.data(), croma, rms);
        detector.processFrame (croma, rms, extractor.getHopSize() / frecuenciaMuestreo);
    }
}
