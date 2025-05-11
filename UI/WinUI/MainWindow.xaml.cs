using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.System;

namespace Ladybird
{
    public sealed partial class MainWindow : Window
    {
        public MainWindow()
        {
            this.InitializeComponent();

            ExtendsContentIntoTitleBar = true;
            SetTitleBar(AppTitleBar);
        }

        private void AddressBar_KeyDown(object sender, KeyRoutedEventArgs e)
        {
            if (e.Key == VirtualKey.Enter)
            {
                var textBox = sender as TextBox;
                var url = textBox?.Text;

                // FIXME: Actually perform navigation request
                System.Diagnostics.Debug.WriteLine($"Navigate to: {url}");
            }
        }

        private void TabView_AddTabButtonClick(TabView sender, object args)
        {
            var newTab = new TabViewItem
            {
                Header = "New Tab",
                IconSource = new BitmapIconSource { UriSource = new Uri("ms-appx:///Assets/Square44x44Logo.png") }
            };

            var tabGrid = new Grid();
            tabGrid.RowDefinitions.Add(new RowDefinition { Height = GridLength.Auto });
            tabGrid.RowDefinitions.Add(new RowDefinition { Height = new GridLength(1, GridUnitType.Star) });

            var addressBarGrid = new Grid { Margin = new Thickness(12, 8, 12, 8) };
            var addressBar = new TextBox
            {
                PlaceholderText = "Enter URL or search term",
                HorizontalAlignment = HorizontalAlignment.Center,
                MinWidth = 600,
                MaxWidth = 800,
                Height = 32,
                CornerRadius = new CornerRadius(4)
            };
            addressBar.KeyDown += AddressBar_KeyDown;
            addressBarGrid.Children.Add(addressBar);
            Grid.SetRow(addressBarGrid, 0);

            var contentGrid = new Grid();
            var contentPanel = new StackPanel
            {
                HorizontalAlignment = HorizontalAlignment.Center,
                VerticalAlignment = VerticalAlignment.Center
            };
            var contentText = new TextBlock
            {
                Text = "Placeholder Web Content",
                Style = Application.Current.Resources["TitleTextBlockStyle"] as Style,
                HorizontalAlignment = HorizontalAlignment.Center
            };
            contentPanel.Children.Add(contentText);
            contentGrid.Children.Add(contentPanel);
            Grid.SetRow(contentGrid, 1);

            tabGrid.Children.Add(addressBarGrid);
            tabGrid.Children.Add(contentGrid);

            newTab.Content = tabGrid;
            sender.TabItems.Add(newTab);
            sender.SelectedItem = newTab;
        }

        private void TabView_TabCloseRequested(TabView sender,
                                               TabViewTabCloseRequestedEventArgs args)
        {
            sender.TabItems.Remove(args.Tab);
            if (sender.TabItems.Count == 0)
            {
                App.Window.Close();
            }
        }
    }
}
