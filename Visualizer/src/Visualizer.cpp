#include <QFileDialog>
#include <QGridLayout>
#include "Visualizer.h"
#include "STLReader.h"
#include "OBJReader.h"
#include "OBJWriter.h"
#include "STLWriter.h"
#include "DataWriter.h"


Visualizer::Visualizer(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi();

    connect(loadFile, &QPushButton::clicked, this, &Visualizer::onLoadFileClick);
    connect(translate, &QPushButton::clicked, this, &Visualizer::onTranslateClick);
    connect(exportFile, &QPushButton::clicked, this, &Visualizer::onExportClick);
}

Visualizer::~Visualizer()
{

}

void Visualizer::setupUi()
{
    loadFile = new QPushButton("Load File", this);
    translate = new QPushButton("Translate", this);
    exportFile= new QPushButton("Export", this);
    openglWidgetInput = new OpenGlWidget(this);
    openglWidgetOutput = new OpenGlWidget(this);

    graphicsSynchronizer = new GraphicsSynchronizer(openglWidgetInput, openglWidgetOutput);

    QGridLayout* layout = new QGridLayout(this);

    layout->addWidget(loadFile, 0, 0);
    layout->addWidget(translate, 0, 1);
    layout->addWidget(openglWidgetInput, 1, 0);
    layout->addWidget(openglWidgetOutput, 1, 1, 1, 2);
    layout->addWidget(exportFile, 0, 2);


    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    centralWidget->setLayout(layout);
}

void  Visualizer::onLoadFileClick()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), "", tr("files (*.stl *.obj)"));

    if (!fileName.isEmpty())
    {
        inputFilePath = fileName;
        if(inputFilePath.endsWith(".stl",Qt::CaseInsensitive))
        {
            STLReader reader;
            reader.read(inputFilePath.toStdString(), triangulation);
        }
        else if (inputFilePath.endsWith(".obj", Qt::CaseInsensitive))
        {
            OBJReader reader;
            reader.read(inputFilePath.toStdString(), triangulation);
        }
        OpenGlWidget::Data data = convertTrianglulationToGraphicsObject(triangulation);
        openglWidgetInput->setData(data);
    }
    else
    {
        msgBox.setText("No file was selected");
        msgBox.exec();
    }
}

void Visualizer::onTranslateClick()
{
    if (!inputFilePath.isEmpty())
    {
        QFileDialog dialog(this);

        dialog.setFileMode(QFileDialog::Directory);

        QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
            "/home",
            QFileDialog::ShowDirsOnly
            | QFileDialog::DontResolveSymlinks);

        // Check if directory is selected
        if (!dir.isEmpty())
        {
            QString exportFileName;

            if (inputFilePath.endsWith(".stl", Qt::CaseInsensitive))
            {
                exportFileName = QDir(dir).filePath("output.obj");
                ObjWriter writer;
                writer.Write(exportFileName.toStdString(), triangulation);

                // reload file to check and load in output renderer
                OBJReader reader;
                reader.read(exportFileName.toStdString(), triangulation);
            }

            else if (inputFilePath.endsWith(".obj", Qt::CaseInsensitive))
            {
                exportFileName = QDir(dir).filePath("output.stl");
                STLWriter writer;
                writer.Write(exportFileName.toStdString(), triangulation);

                // reload file to check and load in output renderer
                STLReader reader;
                reader.read(exportFileName.toStdString(), triangulation);
            }

            OpenGlWidget::Data data = convertTrianglulationToGraphicsObject(triangulation);
            openglWidgetOutput->setData(data);
        }
        else
        {
            msgBox.setText("No directory was selected!");
            msgBox.exec();
        }
    }
    else
    {
        msgBox.setText("Input file was not loaded!");
        msgBox.exec();
    }
}

void Visualizer::onExportClick()
{
    if (!inputFilePath.isEmpty())
    {
        QFileDialog dialog(this);

        if (inputFilePath.endsWith(".stl", Qt::CaseInsensitive))
        {
            QString fileName = QFileDialog::getSaveFileName(this,
                tr("Save File"), "", tr("files (*.obj)"));
            ObjWriter writer;
            writer.Write(fileName.toStdString(), triangulation);
        }
        else if (inputFilePath.endsWith(".obj", Qt::CaseInsensitive))
        {
            QString fileName = QFileDialog::getSaveFileName(this,
                tr("Save File"), "", tr("files (*.stl)"));
            STLWriter writer;
            writer.Write(fileName.toStdString(), triangulation);
        }
    }
    else
    {
        msgBox.setText("Input file was not loaded!");
        msgBox.exec();
    }
}

OpenGlWidget::Data Visualizer::convertTrianglulationToGraphicsObject(const Triangulation& triangulation)
{
    OpenGlWidget::Data data;
    for each (Triangle triangle in triangulation.Triangles)
    {
        for each (Point point in triangle.Points())
        {
            data.vertices.push_back(triangulation.UniqueNumbers[point.X()]);
            data.vertices.push_back(triangulation.UniqueNumbers[point.Y()]);
            data.vertices.push_back(triangulation.UniqueNumbers[point.Z()]);
        }

        Point normal = triangle.Normal();

        for (size_t i = 0; i < 3; i++)
        {
            data.normals.push_back(triangulation.UniqueNumbers[normal.X()]);
            data.normals.push_back(triangulation.UniqueNumbers[normal.Y()]);
            data.normals.push_back(triangulation.UniqueNumbers[normal.Z()]);
        }
    }
    return data;
}
