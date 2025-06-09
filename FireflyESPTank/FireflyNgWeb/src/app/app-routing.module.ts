import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { RouterModule, Routes } from '@angular/router';
import {WebcontrolComponent} from "./webcontrol/webcontrol.component"
const appRoutes: Routes = [
  { path: 'webcontrol', component: WebcontrolComponent },
  { path: '',
    redirectTo: '/webcontrol',
    pathMatch: 'full'
  }
];

@NgModule({
  declarations: [],
  imports: [
    CommonModule,
    RouterModule.forRoot(
      appRoutes,
      { enableTracing: true } // <-- debugging purposes only
    )
  ],
  exports: [RouterModule]
})
export class AppRoutingModule { }
