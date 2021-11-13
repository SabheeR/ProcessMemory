#include "processmemoryexporteditor.h"
#include "ui_processmemoryexporteditor.h"

ProcessMemoryExportEditor::ProcessMemoryExportEditor(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::ProcessMemoryExportEditor()),
    m_paramHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);

    // TODO: Correlate parameters in given delegate to form fields
    // Ex 1.
    // m_paramHelper->addSpinBoxIntParameter("myparametername", ui->spinBox);
    // Ex 2.
    // m_paramHelper->addParameter("otherparameter", [this](QJsonValue value) {
    //     // set the value in the editor
    //     ui->spinBox->setValue(value.toInt());
    //     return true; // return false if unable to set the value in the editor
    // }, [this]() {
    //     // get the QJsonValue from the editor
    //     return QJsonValue(ui->spinBox->value());
    // });
}

ProcessMemoryExportEditor::~ProcessMemoryExportEditor()
{
    delete ui;
}

QString ProcessMemoryExportEditor::title()
{
    // TODO: Make this more descriptive
    return "Configure ProcessMemory";
}

Parameters ProcessMemoryExportEditor::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

bool ProcessMemoryExportEditor::setParameters(const Parameters &parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}

void ProcessMemoryExportEditor::previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                                      QSharedPointer<PluginActionProgress> progress)
{
    // TODO: (Optional) Preview the currently active BitContainer (preprocess it, enrich it, etc)
}

void ProcessMemoryExportEditor::previewBitsUiImpl(QSharedPointer<BitContainerPreview> container)
{
    // TODO: (Optional) Update UI elements to account for preprocessing in previewBitsImpl and/or other metadata
}
