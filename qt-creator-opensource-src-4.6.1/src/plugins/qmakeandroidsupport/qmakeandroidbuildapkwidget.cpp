/****************************************************************************
**
** Copyright (C) 2016 BogDan Vatra <bog_dan_ro@yahoo.com>
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "createandroidmanifestwizard.h"
#include "qmakeandroidbuildapkstep.h"
#include "qmakeandroidbuildapkwidget.h"
#include "ui_qmakeandroidbuildapkwidget.h"

#include <android/androidbuildapkwidget.h>
#include <android/androidmanager.h>

#include <projectexplorer/project.h>

#include <utils/utilsicons.h>

#include <QFileDialog>
#include <QLabel>

namespace QmakeAndroidSupport {
namespace Internal {

QmakeAndroidBuildApkWidget::QmakeAndroidBuildApkWidget(Android::AndroidBuildApkStep *step) :
    m_ui(new Ui::QmakeAndroidBuildApkWidget),
    m_step(step)
{
    QVBoxLayout *topLayout = new QVBoxLayout;

    QHBoxLayout *qt51WarningLayout = new QHBoxLayout();
    QLabel *oldFilesWarningIcon = new QLabel(this);
    oldFilesWarningIcon->setObjectName(QStringLiteral("oldFilesWarningIcon"));
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(oldFilesWarningIcon->sizePolicy().hasHeightForWidth());
    oldFilesWarningIcon->setSizePolicy(sizePolicy);
    oldFilesWarningIcon->setPixmap(Utils::Icons::WARNING.pixmap());
    oldFilesWarningIcon->setAlignment(Qt::Alignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop));
    qt51WarningLayout->addWidget(oldFilesWarningIcon);

    QLabel *oldFilesWarningLabel = new QLabel(this);
    oldFilesWarningLabel->setObjectName(QStringLiteral("oldFilesWarningLabel"));
    oldFilesWarningLabel->setWordWrap(true);
    qt51WarningLayout->addWidget(oldFilesWarningLabel);

    topLayout->addWidget(new Android::AndroidBuildApkWidget(step));

    QWidget *widget = new QWidget(this);
    m_ui->setupUi(widget);
    topLayout->addWidget(widget);
    setLayout(topLayout);

    bool oldFiles = Android::AndroidManager::checkForQt51Files(m_step->project()->projectDirectory());
    oldFilesWarningIcon->setVisible(oldFiles);
    oldFilesWarningLabel->setVisible(oldFiles);

    m_extraLibraryListModel = new AndroidExtraLibraryListModel(m_step->target(), this);
    m_ui->androidExtraLibsListView->setModel(m_extraLibraryListModel);

    connect(m_ui->createAndroidTemplatesButton, &QAbstractButton::clicked,
            this, &QmakeAndroidBuildApkWidget::createAndroidTemplatesButton);

    connect(m_ui->addAndroidExtraLibButton, &QAbstractButton::clicked,
            this, &QmakeAndroidBuildApkWidget::addAndroidExtraLib);

    connect(m_ui->removeAndroidExtraLibButton, &QAbstractButton::clicked,
            this, &QmakeAndroidBuildApkWidget::removeAndroidExtraLib);

    connect(m_ui->androidExtraLibsListView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &QmakeAndroidBuildApkWidget::checkEnableRemoveButton);

    connect(m_extraLibraryListModel, &AndroidExtraLibraryListModel::enabledChanged,
            m_ui->additionalLibrariesGroupBox, &QWidget::setEnabled);

    m_ui->additionalLibrariesGroupBox->setEnabled(m_extraLibraryListModel->isEnabled());
}

QmakeAndroidBuildApkWidget::~QmakeAndroidBuildApkWidget()
{
    delete m_ui;
}

void QmakeAndroidBuildApkWidget::createAndroidTemplatesButton()
{
    CreateAndroidManifestWizard wizard(m_step->target());
    wizard.exec();
}

void QmakeAndroidBuildApkWidget::addAndroidExtraLib()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
                                                          tr("Select additional libraries"),
                                                          QDir::homePath(),
                                                          tr("Libraries (*.so)"));

    if (!fileNames.isEmpty())
        m_extraLibraryListModel->addEntries(fileNames);
}

void QmakeAndroidBuildApkWidget::removeAndroidExtraLib()
{
    QModelIndexList removeList = m_ui->androidExtraLibsListView->selectionModel()->selectedIndexes();
    m_extraLibraryListModel->removeEntries(removeList);
}

void QmakeAndroidBuildApkWidget::checkEnableRemoveButton()
{
    m_ui->removeAndroidExtraLibButton->setEnabled(m_ui->androidExtraLibsListView->selectionModel()->hasSelection());
}

QString QmakeAndroidBuildApkWidget::summaryText() const
{
    return tr("<b>Build Android APK</b>");
}

QString QmakeAndroidBuildApkWidget::displayName() const
{
    return summaryText();
}

} // namespace Internal
} // namespace QmakeAndroidSupport


