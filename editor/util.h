#pragma once

#include <QtCore/QPoint>
#include <QtCore/QString>
#include <QtGui/QColor>
#include <cassert>

#include "model/Value.h"

#define unreachable \
    { \
        assert(false); \
        abort(); \
    }

namespace AxiomUtil {

    QString loadStylesheet(const char *path);

    QPoint clampP(QPoint p, QPoint min, QPoint max);

    QColor mixColor(QColor a, QColor b, float mix);

    bool strToFloat(const QString &str, float &result);

    QString getNoteName(float noteVal);

    QString formatFloatForm(float val, AxiomModel::FormType form);

    const char *getFormUnit(float val, AxiomModel::FormType form);

    QString formatChannelFull(float val, AxiomModel::FormType form);

    QString formatNumForm(AxiomModel::NumValue value, bool includeForm);

    template<class InputIterator, class T>
    InputIterator findUnique(InputIterator first, InputIterator last, const T *ptr) {
        while (first != last) {
            if (first->get() == ptr) return first;
            ++first;
        }
        return last;
    };
}
