using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

using ZedGraph;
using DUTConfig_V3.Settings;

namespace DUTConfig_V3.Forms
{
    public partial class GraphControl : UserControl
    {
        ZedGraphControl _graphControl;

        public string XAxisLabel 
        {
            get
            {
                return _graphControl.GraphPane.XAxis.Title.Text;
            }
            set
            {
                _graphControl.GraphPane.XAxis.Title.Text = value;
            }
        }
        public string YAxisLabel 
        {
            get
            {
                return _graphControl.GraphPane.YAxis.Title.Text;
            }
            set
            {
                _graphControl.GraphPane.YAxis.Title.Text = value;
            }
        }
        public string Title 
        {
            get
            {
                return _graphControl.GraphPane.Title.Text;
            }
            set
            {
                _graphControl.GraphPane.Title.Text = value;
            }
        }

        public Color Color1 { get; set; }
        public Color Color2 { get; set; }

        public int Width1 { get; set; }
        public int Width2 { get; set; }

        public SymbolType NodeSymbol1 { get; set; }
        public SymbolType NodeSymbol2 { get; set; }

        public bool UseSmoothing { get; set; }

        public GraphControl()
        {
            InitializeComponent();

            _graphControl = new ZedGraphControl();
            _graphControl.Dock = DockStyle.Fill;

            this.Controls.Add(_graphControl);
            _graphControl.GraphPane.XAxis.MajorGrid.IsVisible = true;
            _graphControl.GraphPane.YAxis.MajorGrid.IsVisible = true;

            _graphControl.GraphPane.XAxis.Scale.MajorStepAuto = true;
            _graphControl.GraphPane.YAxis.Scale.MajorStepAuto = true;

            _graphControl.GraphPane.IsBoundedRanges = true;

            Color1 = Color.Blue;
            Color2 = Color.Red;

            Width1 = 2;
            Width2 = 3;

            NodeSymbol1 = SymbolType.None;
            NodeSymbol2 = SymbolType.None;

            UseSmoothing = false;
        }

        public void TranslateMenu(Translation trans)
        {
            _graphControl.ContextMenuBuilder += new ZedGraphControl.ContextMenuBuilderEventHandler((s, ms, mp, o) =>
            {
                ms.Items.RemoveAt(6);  // undo all zoom/pan
                ms.Items.RemoveAt(0);  // copy

                ms.Items[0].Text = trans.GraphMenuSaveAs;
                ms.Items[1].Text = trans.GraphMenuPageSetup;
                ms.Items[2].Text = trans.GraphMenuPrint;
                ms.Items[3].Text = trans.GraphMenuPointValues;
                ms.Items[4].Text = trans.GraphMenuUnZoom;
                ms.Items[5].Text = trans.GraphMenuDefaultScale;
            });
        }

        public void Clear()
        {
            GraphPane g = _graphControl.GraphPane;
            g.CurveList.Clear();
        }

        public void DrawCurve1(string label, PointPairList list)
        {
            GraphPane g = _graphControl.GraphPane;

            LineItem l = g.AddCurve(label, list, Color1, NodeSymbol1);
            l.Line.Width = Width1;
            l.Line.IsSmooth = UseSmoothing;
            
            _graphControl.AxisChange();
            _graphControl.Invalidate();
        }

        public void DrawCurve2(string label, PointPairList list)
        {
            GraphPane g = _graphControl.GraphPane;

            LineItem l = g.AddCurve(label, list, Color2, NodeSymbol2);
            l.Line.Width = Width2;
            l.Line.IsSmooth = UseSmoothing;

            _graphControl.AxisChange();
            _graphControl.Invalidate();
        }
    }
}
